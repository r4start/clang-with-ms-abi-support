//===--- MicrosoftCGException.cpp - Emit LLVM Code for C++ exceptions -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This contains code dealing with Microsoft C++ exception related code 
// generation.
//
//===----------------------------------------------------------------------===//

#include "CodeGenFunction.h"
#include "CGCleanup.h"
#include "CGObjCRuntime.h"
#include "TargetInfo.h"
#include "CGCXXABI.h"
#include "clang/AST/StmtCXX.h"
#include "llvm/Intrinsics.h"
#include "llvm/Support/CallSite.h"
#include "llvm/InlineAsm.h"

using namespace clang;
using namespace CodeGen;

// r4start
static llvm::BasicBlock *getEHHandler(CodeGenFunction &CGF) {
  const FunctionDecl *func = cast_or_null<FunctionDecl>(CGF.CurFuncDecl);
  assert(func && "EH handler can be generated only for function or method!");

  llvm::SmallString<256> nameBuffer;
  llvm::raw_svector_ostream nameStream(nameBuffer);

  CGF.CGM.getCXXABI().getMangleContext().getMsExtensions()->
    mangleEHHandlerFunction(func, nameStream);

  nameStream.flush();
  StringRef mangledName(nameBuffer);

  return llvm::BasicBlock::Create(CGF.CGM.getLLVMContext(),
                                  mangledName, CGF.CurFn);
}

// r4start
void CodeGenFunction::MSEHState::InitMSTryState() {
  if (!MSTryState) {
    llvm::Function *frameAddr = 
      CGF.CGM.getIntrinsic(llvm::Intrinsic::frameaddress);
    llvm::CallInst *ebp = 
      CGF.Builder.CreateCall(frameAddr, 
                             llvm::ConstantInt::get(CGF.Int32Ty, 0));
    MSTryState = 
      CGF.Builder.CreateGEP(ebp, llvm::ConstantInt::get(CGF.Int32Ty, -4));
    MSTryState = 
      CGF.Builder.CreateBitCast(MSTryState,
           llvm::IntegerType::get(CGF.CGM.getLLVMContext(), 32)->getPointerTo(),
           "try.id");
    EHHandler = getEHHandler(CGF);
  }
}

static llvm::GlobalValue *getDtor(CodeGenModule &CGM, const CXXRecordDecl *Class) {
  if (!Class->hasUserDeclaredDestructor()) {
    return 0;
  }
  return CGM.GetAddrOfCXXDestructor(Class->getDestructor(), Dtor_Base);
}

// r4start
void CodeGenFunction::MSEHState::SetMSTryState() {
  InitMSTryState();

  if (LocalUnwindTable.empty()) {
    LocalUnwindTable.push_back(UnwindEntryRefList());
  }

  size_t state = GlobalUnwindTable.size();
  if (!state) {
    GlobalUnwindTable.push_back(-1);
  } else {
    GlobalUnwindTable.push_back(GlobalUnwindTable.back().StoreValue);
  }

  MsUnwindInfo &backRef = GlobalUnwindTable.back();
  
  LocalUnwindTable.back().push_back(--GlobalUnwindTable.end());

  backRef.Store = CreateStateStore(state);
  backRef.StoreValue = state;
  backRef.StoreInstTryLevel = TryLevel;
  backRef.TryNumber = TryNumber;
  backRef.StoreIndex = StoreIndex++;
}

// r4start
llvm::StoreInst *CodeGenFunction::MSEHState::CreateStateStore(uint32_t State) {
  llvm::StoreInst *store =  
    CGF.Builder.CreateStore(llvm::ConstantInt::get(CGF.Int32Ty, State),
                            MSTryState);
  return store;
}

llvm::StoreInst *CodeGenFunction::MSEHState::CreateStateStore(llvm::Value *State) {
  llvm::StoreInst *store = CGF.Builder.CreateStore(State, MSTryState);
  return store;
}

// r4start
llvm::StoreInst *
CodeGenFunction::MSEHState::CreateStateStoreWithoutEmit(llvm::Value *State) {
  llvm::StoreInst *store = new llvm::StoreInst(State, MSTryState);
  return store;
}

void CodeGenFunction::MSEHState::AddCatchEntryInUnwindTable(int State) {
  GlobalUnwindTable.push_back(State);
  GlobalUnwindTable.back().StoreValue = State;

  // TryNumber must be right because it necessary for GenerateTryBlockEntry.
  // If TryNumber is wrong then GenerateTryBlockEntry can not find this entry
  // in global unwind table.
  if (!LocalUnwindTable.back().empty()) {
    GlobalUnwindTable.back().TryNumber = 
      (*(LocalUnwindTable.back().begin()))->TryNumber;
  } else {
    GlobalUnwindTable.back().TryNumber = TryNumber;
  }

  GlobalUnwindTable.back().StoreInstTryLevel = TryLevel;
  GlobalUnwindTable.back().StoreIndex = StoreIndex++;
  GlobalUnwindTable.back().RestoreKind = RestoreOpInfo::CatchRestore;
}

void CodeGenFunction::MSEHState::SaveStackPointer() {
  if (GlobalUnwindTable.empty()) {
    return;
  }

  if (!SaveSPIntrinsic) {
    SaveSPIntrinsic = CGF.CGM.getIntrinsic(llvm::Intrinsic::seh_esp_save);
  }

  CGF.Builder.CreateCall(SaveSPIntrinsic);
}

llvm::BasicBlock *
CodeGenFunction::MSEHState::GenerateTryEndBlock(const FunctionDecl *FD, 
                                          MSMangleContextExtensions *Mangler) {
  assert(FD);
  assert(Mangler);
  
  llvm::SmallString<256> tryEndName;
  llvm::raw_svector_ostream stream(tryEndName);

  Mangler->mangleEHTryEnd(FD, EHManglingCounter, stream);
  EHManglingCounter++;

  stream.flush();
  StringRef tryEndNameRef(tryEndName);

  return llvm::BasicBlock::Create(CGF.CGM.getLLVMContext(),
                                  tryEndNameRef, CGF.CurFn);
}

// r4start
static llvm::Constant *getFakePersonality(CodeGenFunction &CGF) {
  llvm::Constant *Fn =
    CGF.CGM.CreateRuntimeFunction(
                              llvm::FunctionType::get(CGF.CGM.Int32Ty, true),
                              "ms_fake_personality");
  return Fn;
}

// r4start
static llvm::BasicBlock *getLPadBlock(CodeGenFunction &CGF) {
  // Create and configure the landing pad.
  llvm::BasicBlock *lpadBlock = CGF.createBasicBlock("lpad");
  
  // Save the current IR generation state.
  CGBuilderTy::InsertPoint savedIP = CGF.Builder.saveAndClearIP();

  CGF.EmitBlock(lpadBlock);

  llvm::LandingPadInst *lPad = 
      CGF.Builder.CreateLandingPad(
                llvm::StructType::get(CGF.Int8PtrTy, CGF.Int32Ty, NULL),
                llvm::ConstantExpr::getBitCast(getFakePersonality(CGF),
                                               CGF.CGM.Int8PtrTy),
                0);
  lPad->setCleanup(true);
  CGF.Builder.CreateUnreachable();
  // Restore the old IR generation state.
  CGF.Builder.restoreIP(savedIP);
  return lpadBlock;
}

// r4start
static llvm::Constant *getNullPointer(llvm::Type *Type) {
  return llvm::ConstantPointerNull::get(cast<llvm::PointerType>(Type)); 
}

// r4start
static llvm::Constant *getMSThrowFn(CodeGenFunction &CGF, 
                                    llvm::Type *ThrowInfoPtrTy) {
  if (llvm::Function *throwFn = 
            CGF.CGM.getModule().getFunction("_CxxThrowException")) {
    return throwFn;
  }

  llvm::Type *Args[2] = { CGF.Int8PtrTy, ThrowInfoPtrTy };
  llvm::FunctionType *FTy =
    llvm::FunctionType::get(CGF.VoidTy, Args, /*IsVarArgs=*/false);

  llvm::Function *throwFn = 
    cast<llvm::Function>(CGF.CGM.CreateRuntimeFunction(FTy,
                                             "_CxxThrowException"));
  throwFn->setCallingConv(llvm::CallingConv::X86_StdCall);
  return throwFn;
}

// r4start
static llvm::Function *getMSFrameHandlerFunction(CodeGenFunction &CGF, 
                                                 llvm::Type *EHFuncInfoTy) {
  if (llvm::Function *handler = 
            CGF.CGM.getModule().getFunction("__CxxFrameHandler3")) {
    return handler;
  }

  llvm::Type *Args[1] = { EHFuncInfoTy };
  llvm::FunctionType *FTy = 
    llvm::FunctionType::get(CGF.VoidTy, Args, /*IsVarArgs=*/false);

  llvm::Function *F = llvm::Function::Create(FTy,
                                             llvm::GlobalValue::ExternalLinkage, 
                                             "__CxxFrameHandler3",
                                             &CGF.CGM.getModule());
  F->addAttribute(1, llvm::Attribute::InReg | llvm::Attribute::NoCapture);
  return F;
}

// r4start
static llvm::StructType *getOrCreateCatchableType(CodeGenModule &CGM) {
  if (llvm::StructType *catchablesType = 
                           CGM.getModule().getTypeByName("catchable.type")) {
    return catchablesType;
  }

  llvm::SmallVector<llvm::Type *, 5> types;
  
  types.push_back(CGM.Int32Ty);
  // type_info type is void*.
  types.push_back(CGM.GetDescriptorPtrType(CGM.Int8PtrTy));
  types.push_back(CGM.GetPMDtype());
  types.push_back(CGM.Int32Ty);
  types.push_back(llvm::FunctionType::get(CGM.VoidTy, false)->getPointerTo());

  return llvm::StructType::create(CGM.getLLVMContext(), types,
                                  "catchable.type");
}

static llvm::Constant *getPMDInit(CodeGenModule &CGM, int mdisp,
                                  int pdisp, int vdisp) {
  llvm::SmallVector<llvm::Constant *, 3> vals;
  vals.push_back(llvm::ConstantInt::get(CGM.Int32Ty, mdisp));
  vals.push_back(llvm::ConstantInt::get(CGM.Int32Ty, pdisp));
  vals.push_back(llvm::ConstantInt::get(CGM.Int32Ty, vdisp));

  llvm::StructType *pmdTy = CGM.GetPMDtype();
  return llvm::ConstantStruct::get(pmdTy, vals);
}

// r4start
//  struct CatchableType {
//    // 0x01: simple type (can be copied by memmove), 
//    // 0x02: can be caught by reference only, 0x04: has virtual bases
//    DWORD properties;
//
//    // see above
//    TypeDescriptor* pType;
//
//    // how to cast the thrown object to this type
//    PMD thisDisplacement;
//
//    // object size
//    int sizeOrOffset;
//
//    // copy constructor address
//    void (*copyFunction)();
//  };
static llvm::Constant *getCatchable(CodeGenModule &CGM, 
                                    QualType CatchableType) {
  llvm::SmallString<256> nameBuffer;
  llvm::raw_svector_ostream stream(nameBuffer);

  MSMangleContextExtensions *msExt = 
    CGM.getCXXABI().getMangleContext().getMsExtensions();

  assert(msExt && "Catchable meaningful only in MS C++ ABI!");
  msExt->mangleCatchTypeElement(CatchableType, stream);
  stream.flush();

  llvm::StringRef mangledName(nameBuffer);

  if (llvm::Constant *elem = CGM.getModule().getGlobalVariable(mangledName)) {
    return elem;
  }

  int properties = 0;
  int sizeOrOffset = 0;
  llvm::Constant *ctorAddr = 0;
  llvm::StructType *catchableTy = getOrCreateCatchableType(CGM);

  // 0x01: simple type (can be copied by memmove)
  if (CatchableType.isCXX98PODType(CGM.getContext())) {
    properties |= 1;
  }

  if (const CXXRecordDecl *caught = CatchableType->getAsCXXRecordDecl()) {

    // TODO: 0x02: can be caught by reference only
    // When ?

    // 0x04: has virtual bases
    if (caught->getNumVBases()) {
      properties |= 4;
    }

    const ASTRecordLayout &Layout = 
      caught->getASTContext().getASTRecordLayout(caught);
    sizeOrOffset = Layout.getSize().getQuantity();

    if (caught->hasUserDeclaredCopyConstructor()) {
      const CXXConstructorDecl *copyCtorDecl = 0;
    
      // replace with find_if.
      for (CXXRecordDecl::method_iterator M = caught->method_begin(),
           E = caught->method_end(); M != E; ++M) {
        if (isa<CXXConstructorDecl>(*M)) {
          const CXXConstructorDecl *ctor = cast<CXXConstructorDecl>(*M);
          if (ctor->isCopyConstructor()) {
            copyCtorDecl = ctor;
            break;
          }
        }
      }

      ctorAddr = CGM.GetAddrOfCXXConstructor(copyCtorDecl,
                                             Ctor_Base);
      ctorAddr = llvm::ConstantExpr::getBitCast(ctorAddr,
                                         catchableTy->getStructElementType(4));
    } else {
      ctorAddr = getNullPointer(catchableTy->getStructElementType(4));
    }
  } else {
    ctorAddr = getNullPointer(catchableTy->getStructElementType(4));
    // FIXME: remove 8 with size of byte in bits.
    sizeOrOffset = CGM.getContext().getTypeInfo(CatchableType).first / 8;
  }

  llvm::Constant *typeDescr = CGM.GetAddrOfMSTypeDescriptor(CatchableType);

  SmallVector<llvm::Constant *, 5> catchableEntry;

  catchableEntry.push_back(llvm::ConstantInt::get(CGM.Int32Ty, properties));
  catchableEntry.push_back(
    llvm::ConstantExpr::getBitCast(typeDescr, 
                                   CGM.GetDescriptorPtrType(CGM.Int8PtrTy)));
  catchableEntry.push_back(getPMDInit(CGM, 0, -1, 0));

  
  catchableEntry.push_back(
    llvm::ConstantInt::get(CGM.Int32Ty, sizeOrOffset));
  
  catchableEntry.push_back(ctorAddr);

  llvm::Constant *init = llvm::ConstantStruct::get(catchableTy, catchableEntry);
  return new llvm::GlobalVariable(CGM.getModule(), init->getType(), false,
                                  llvm::GlobalValue::WeakAnyLinkage, 
                                  init, mangledName);
}

// r4start
//  struct CatchableTypeArray {
//    // number of entries in the following array
//    int nCatchableTypes; 
//    CatchableType* arrayOfCatchableTypes[0];
//  };
static llvm::StructType *getCatchableArrayType(CodeGenModule &CGM) {
  if (llvm::StructType *cta = 
            CGM.getModule().getTypeByName("catchable.array.type")) {
    return cta;
  }
  llvm::SmallVector<llvm::Type *, 2> types;
  types.push_back(CGM.Int32Ty);

  llvm::StructType *catchableType = getOrCreateCatchableType(CGM);

  types.push_back(llvm::ArrayType::get(catchableType->getPointerTo(), 0));

  return llvm::StructType::create(CGM.getLLVMContext(), types,
                                  "catchable.array.type");
}

// r4start
//  struct ThrowInfo {
//    // 0x01: const, 0x02: volatile
//    DWORD attributes;
//
//    // exception destructor
//    void (*pmfnUnwind)();
//
//    // forward compatibility handler
//    int (*pForwardCompat)();
//
//    // list of types that can catch this exception.
//    // i.e. the actual type and all its ancestors.
//    CatchableTypeArray* pCatchableTypeArray;
//  };
static llvm::StructType *getThrowInfoType(CodeGenModule &CGM) {
  if (llvm::StructType *tiType = 
          CGM.getModule().getTypeByName("throw.info.type")) {
    return tiType;
  }

  llvm::SmallVector<llvm::Type *, 4> types;

  types.push_back(CGM.Int32Ty);

  types.push_back(llvm::FunctionType::get(CGM.VoidTy, false)->getPointerTo());
  types.push_back(llvm::FunctionType::get(CGM.Int32Ty, false)->getPointerTo());

  types.push_back(getCatchableArrayType(CGM)->getPointerTo());

  return llvm::StructType::create(CGM.getLLVMContext(), types,
                                  "throw.info.type");
}

namespace {

template<typename VectorTy>
class CatchableGenerator {
  VectorTy &container;
  CodeGenModule &cgm;
  llvm::Type *catchableType;

public:
  CatchableGenerator(CodeGenModule &CGM, VectorTy &Accumulator) 
   : container(Accumulator), cgm(CGM), 
     catchableType(getOrCreateCatchableType(cgm)->getPointerTo()) {}
  
  void operator() (QualType Type) {
    container.push_back(getCatchable(cgm, Type));
  }
};

template<typename ContainerTy>
void discoverAllCatchabletypes(const CXXRecordDecl *Class, 
                               ContainerTy &Accum) {
  Accum.push_back(Class->getASTContext().getRecordType(Class));

  if (!Class->getNumBases()) {
    return;
  }

  for (CXXRecordDecl::base_class_const_iterator I = Class->bases_begin(),
       E = Class->bases_end(); I != E; ++I) {
    discoverAllCatchabletypes(I->getType()->getAsCXXRecordDecl(), Accum);
  }
}

}

// r4start
static llvm::Constant *generateCatchableArrayInit(CodeGenModule &CGM, 
                                                  QualType ThrowType) {
  MSMangleContextExtensions *extensions = 
    CGM.getCXXABI().getMangleContext().getMsExtensions();
  
  assert(extensions && "Not in MS mode!");
  
  llvm::SmallString<256> name;
  llvm::raw_svector_ostream stream(name);

  int numberOfBases = 1;
  if (const CXXRecordDecl *catchDecl = ThrowType->getAsCXXRecordDecl()) {
    numberOfBases += catchDecl->getNumBases();
  }

  extensions->mangleCatchTypeArray(ThrowType, 
                                   numberOfBases,
                                   stream);
  stream.flush();
  llvm::StringRef mangledName(name);

  llvm::GlobalVariable *gv = 
    CGM.getModule().getGlobalVariable(mangledName);
  if (gv) {
    return gv;
  }

  typedef llvm::SmallVector<llvm::Constant *, 8> InitContainerTy;
  typedef llvm::SmallVector<llvm::Type *, 2> CTATypes;
  typedef llvm::SmallVector<QualType, 8> CatchablesContainerTy;

  CatchablesContainerTy catchableTypes;
  if (const CXXRecordDecl *throwDecl = ThrowType->getAsCXXRecordDecl()) {
    discoverAllCatchabletypes<CatchablesContainerTy>(throwDecl, catchableTypes);
  } else {
    catchableTypes.push_back(ThrowType);
  }

  InitContainerTy init;
  std::for_each(catchableTypes.begin(), catchableTypes.end(), 
                CatchableGenerator<InitContainerTy>(CGM, init));
  
  assert(numberOfBases == init.size() && 
         "Array size must be equal bases count!");

  llvm::Type *catchableTy = getOrCreateCatchableType(CGM)->getPointerTo();
  
  llvm::ArrayType *arrTy = 
    llvm::ArrayType::get(catchableTy, numberOfBases);

  llvm::Constant *arr = llvm::ConstantArray::get(arrTy, init);

  llvm::Constant *number = llvm::ConstantInt::get(CGM.Int32Ty, numberOfBases);
  
  CTATypes fieldTypes;
  fieldTypes.push_back(number->getType());
  fieldTypes.push_back(arr->getType());

  init.clear();
  init.push_back(number);
  init.push_back(arr);

  llvm::StructType *ctaType = 
    llvm::StructType::get(CGM.getLLVMContext(), fieldTypes);

  llvm::Constant *cta = llvm::ConstantStruct::get(ctaType, init);

  return
    new llvm::GlobalVariable(CGM.getModule(), cta->getType(), false,
                             llvm::GlobalValue::WeakAnyLinkage,
                             cta, mangledName);
}

// r4start
static llvm::Constant *generateThrowInfoInit(CodeGenFunction &CGF, 
                                             QualType ThrowTy,
                                             llvm::StructType *ThrowInfoTy) {
  const CXXRecordDecl *throwTypeDecl = ThrowTy->getAsCXXRecordDecl();
  llvm::GlobalValue *dtorPtr = 0;
  if (throwTypeDecl) {
      dtorPtr = getDtor(CGF.CGM, throwTypeDecl);
  }
  
  llvm::SmallVector<llvm::Constant *, 4> initVals;

  initVals.push_back(llvm::ConstantInt::get(CGF.CGM.Int32Ty, 0));

  if (dtorPtr) {
    initVals.push_back(llvm::ConstantExpr::getBitCast(dtorPtr,
                                        ThrowInfoTy->getStructElementType(1)));
  } else {
    initVals.push_back(llvm::ConstantPointerNull::get(
               cast<llvm::PointerType>(ThrowInfoTy->getStructElementType(1))));
  }

  llvm::Constant *forwardComp = 
    llvm::ConstantPointerNull::get(
          cast<llvm::PointerType>(ThrowInfoTy->getStructElementType(2)));
  initVals.push_back(forwardComp);

  llvm::SmallString<256> name;
  llvm::raw_svector_ostream stream(name);

  llvm::Constant *cta = generateCatchableArrayInit(CGF.CGM, ThrowTy);

  initVals.push_back(
    llvm::ConstantExpr::getBitCast(cta, ThrowInfoTy->getStructElementType(3)));

  return llvm::ConstantStruct::get(ThrowInfoTy, initVals);
}

// r4start
// This function is generate throw info for catch type.
static llvm::GlobalVariable *getOrGenerateThrowInfo(CodeGenFunction &CGF,
                                                    QualType CatchType) {
  MSMangleContextExtensions *msMangler = 
    CGF.CGM.getCXXABI().getMangleContext().getMsExtensions();

  llvm::SmallString<256> buffer;
  llvm::raw_svector_ostream out(buffer);

  int numberOfBases = 1;
  if (const CXXRecordDecl *catchDecl = CatchType->getAsCXXRecordDecl()) {
    numberOfBases += catchDecl->getNumBases();
  }

  msMangler->mangleThrowInfo(CatchType, numberOfBases, out);
  out.flush();

  StringRef throwInfo(buffer);

  llvm::GlobalVariable *gv = 
    CGF.CGM.getModule().getGlobalVariable(throwInfo, true);
  if (gv) {
    return gv;
  }

  llvm::StructType *throwInfoTy = getThrowInfoType(CGF.CGM);
  llvm::Constant *throwInfoInit = 
    generateThrowInfoInit(CGF, CatchType, throwInfoTy);

  return new llvm::GlobalVariable(CGF.CGM.getModule(), throwInfoTy, false,
                                  llvm::GlobalValue::WeakAnyLinkage,
                                  throwInfoInit, throwInfo);
}

// r4start
void CodeGenFunction::EmitMSCXXThrowExpr(const CXXThrowExpr *E) {
  llvm::Value *CXXThrowExParam = 0;
  llvm::Value *ThrowInfo = 0;

  if (E->getSubExpr()) {
    QualType ThrowType = E->getSubExpr()->getType();

    uint64_t ThrowTypeSize = 
      getContext().getTypeSizeInChars(ThrowType).getQuantity();

    llvm::Type *throwTy = CGM.getTypes().ConvertType(ThrowType);

    llvm::AllocaInst *ThrowObj = Builder.CreateAlloca(throwTy, 0,
                                                      "throw.object");

    // If need emits ctor call.
    EmitAnyExprToMem(E->getSubExpr(), ThrowObj,
                     E->getSubExpr()->getType().getQualifiers(), true);

    CXXThrowExParam = Builder.CreateBitCast(ThrowObj, CGM.Int8PtrTy);

    ThrowInfo = getOrGenerateThrowInfo(*this, ThrowType);
  } else {
    // rethrow
    ThrowInfo = 
      llvm::ConstantPointerNull::get(getThrowInfoType(CGM)->getPointerTo());
    CXXThrowExParam = llvm::ConstantPointerNull::get(Int8PtrTy);
  }

  llvm::Value *Params[] = { CXXThrowExParam, ThrowInfo };
                        
  llvm::BasicBlock *normalDest = 
    llvm::BasicBlock::Create(CGM.getLLVMContext(), "", CurFn);
  
  llvm::BasicBlock *invokeDest = getInvokeDest();

  if (invokeDest) {
    llvm::InvokeInst *invoke = 
      Builder.CreateInvoke(getMSThrowFn(*this, ThrowInfo->getType()),
                           normalDest, invokeDest, Params);
    invoke->setCallingConv(llvm::CallingConv::X86_StdCall);
  } else {
    llvm::CallInst *call = 
      Builder.CreateCall(getMSThrowFn(*this, ThrowInfo->getType()), Params);
    call->setCallingConv(llvm::CallingConv::X86_StdCall);
    Builder.CreateUnreachable();
  }
  
  Builder.SetInsertPoint(normalDest);
}

// r4start
//  struct HandlerType {
//    // 0x01: const, 0x02: volatile, 0x08: reference
//    DWORD adjectives;
//
//    // RTTI descriptor of the exception type. 0=any (ellipsis)
//    TypeDescriptor* pType;
//
//    // ebp-based offset of the exception object in the function stack.
//    // 0 = no object (catch by type)
//    int dispCatchObj;
//
//    // address of the catch handler code.
//    // returns address where to continues execution 
//    // (i.e. code after the try block)
//    void* addressOfHandler;
//  };
static llvm::StructType *getHandlerType(CodeGenModule &CGM) {
  if (llvm::StructType *handlerTy = 
          CGM.getModule().getTypeByName("handler.type")) {
    return handlerTy;
  }
  SmallVector<llvm::Type *, 4> fields;

  fields.push_back(CGM.Int32Ty);

  // Type descriptor.
  fields.push_back(CGM.GetDescriptorPtrType(CGM.Int8PtrTy));

  fields.push_back(CGM.Int32Ty);

  fields.push_back(CGM.VoidPtrTy);

  return llvm::StructType::create(CGM.getLLVMContext(), 
                                 fields, "handler.type");
}

// r4start
//  struct ESTypeList {
//    // number of entries in the list
//    int nCount;
//
//    // list of exceptions; it seems only pType field in HandlerType is used
//    HandlerType* pTypeArray;
//  };
static llvm::Type *getESListType(CodeGenModule &CGM) {
  if (llvm::Type *esType = CGM.getModule().getTypeByName("estypelist")) {
    return esType;
  }

  llvm::SmallVector<llvm::Type *, 2> fieldTypes;

  fieldTypes.push_back(CGM.Int32Ty);

  fieldTypes.push_back(getHandlerType(CGM)->getPointerTo());

  return llvm::StructType::create(CGM.getLLVMContext(), fieldTypes, 
    "estypelist");
}

static llvm::Constant *getTypeHandlerForESList(CodeGenModule &CGM,
                                               llvm::Type *HandlerTy,
                                               QualType &ExceptionType) {
  ExceptionType = ExceptionType.getNonReferenceType().getUnqualifiedType();
  llvm::Constant *TypeDescriptor = 
    CGM.GetAddrOfMSTypeDescriptor(ExceptionType);

  llvm::SmallVector<llvm::Constant *, 4> fields;

  // adjectives
  fields.push_back(llvm::ConstantInt::get(CGM.Int32Ty, 0));

  // pTypeDescr
  fields.push_back(llvm::ConstantExpr::getBitCast(TypeDescriptor,
    CGM.GetDescriptorPtrType(CGM.Int8PtrTy)));

  // dispatch obj
  fields.push_back(llvm::ConstantInt::get(CGM.Int32Ty, 0));

  // address of handler
  fields.push_back(llvm::UndefValue::get(CGM.VoidPtrTy));

  llvm::Constant *handler = 
    llvm::ConstantStruct::get(cast<llvm::StructType>(HandlerTy), fields);

  return handler;
}

//  r4start
//  struct TryBlockMapEntry {
//    int tryLow;
//    
//    // this try {} covers states ranging from tryLow to tryHigh
//    int tryHigh;
//
//    // highest state inside catch handlers of this try
//    int catchHigh;
//
//    // number of catch handlers
//    int nCatches;
//
//    //catch handlers table
//    HandlerType* pHandlerArray;
//  };
static llvm::StructType *getTryBlockMapEntryTy(CodeGenModule &CGM) {
  if (llvm::Type *ty = CGM.getModule().getTypeByName("tryblock.map.entry")) {
    return cast<llvm::StructType>(ty);
  }
  llvm::SmallVector<llvm::Type *, 5> mapEntryTypes;

  for (int i = 0; i < 4; ++i) {
    mapEntryTypes.push_back(CGM.Int32Ty);
  }

  mapEntryTypes.push_back(getHandlerType(CGM)->getPointerTo());

  return llvm::StructType::create(CGM.getLLVMContext(), mapEntryTypes,
                                  "tryblock.map.entry");
}

// r4start
// This function is generate type for UnwindMapEntry structure.
//  struct UnwindMapEntry {
//    int toState;        // target state
//    void (*action)();   // action to perform (unwind funclet address)
//  };
static llvm::StructType *getUnwindMapEntryTy(CodeGenModule &CGM) {
  if (llvm::Type *mapEntryTy = 
                            CGM.getModule().getTypeByName("unwind.map.entry")) {
    return cast<llvm::StructType>(mapEntryTy);
  }

  llvm::SmallVector<llvm::Type *, 2> fieldTypes;
  
  fieldTypes.push_back(CGM.Int32Ty);

  fieldTypes.push_back(CGM.VoidPtrTy);

  return llvm::StructType::create(CGM.getLLVMContext(), fieldTypes,
                                  "unwind.map.entry");
}

// r4start
// This function is generate __ehfuncinfo type.
//
// struct FuncInfo {
//    // compiler version.
//    // 0x19930520: up to VC6, 0x19930521: VC7.x(2002-2003), 
//    // 0x19930522: VC8 (2005), VC10 (2010)
//    DWORD magicNumber;
//
//    // number of entries in unwind table
//    int maxState;
//
//    // table of unwind destructors
//    UnwindMapEntry* pUnwindMap;
//
//    // number of try blocks in the function
//    DWORD nTryBlocks;
//
//    // mapping of catch blocks to try blocks
//    TryBlockMapEntry* pTryBlockMap;
//
//    // not used on x86
//    DWORD nIPMapEntries;
//
//    // not used on x86
//    void* pIPtoStateMap;
//
//    // VC7+ only, expected exceptions list (function "throw" specifier) 
//    ESTypeList* pESTypeList;
//
//    // VC8+ only, bit 0 set if function was compiled with /EHs
//    int EHFlags;
//  };
static llvm::StructType *generateEHFuncInfoType(CodeGenModule &CGM) {
  if (llvm::StructType *infoTy = CGM.getModule().getTypeByName("ehfuncinfo")) {
    return infoTy;
  }

  llvm::SmallVector<llvm::Type *, 9> ehfuncinfoFields;

  // magic number.
  ehfuncinfoFields.push_back(CGM.Int32Ty);

  // bbtFlags ??
  //ehfuncinfoFields.push_back(CGM.Int32Ty);

  // max state.
  ehfuncinfoFields.push_back(CGM.Int32Ty);

  // UnwindMapEntry* .
  ehfuncinfoFields.push_back(getUnwindMapEntryTy(CGM)->getPointerTo());

  // try blocks count.
  ehfuncinfoFields.push_back(CGM.Int32Ty);

  // TryBlockMapEntry*.
  ehfuncinfoFields.push_back(getTryBlockMapEntryTy(CGM)->getPointerTo());

  // nIPMapEntries.
  ehfuncinfoFields.push_back(CGM.Int32Ty);

  // not used
  ehfuncinfoFields.push_back(CGM.VoidPtrTy);

  // ESTypeList*.
  ehfuncinfoFields.push_back(getESListType(CGM)->getPointerTo());

  // EHFlags.
  ehfuncinfoFields.push_back(CGM.Int32Ty);

  return llvm::StructType::create(CGM.getLLVMContext(),
                                  ehfuncinfoFields, "ehfuncinfo");
}

// r4start
void CodeGenFunction::EmitESTypeList(const FunctionProtoType *FuncProto) {
  assert(FuncProto && "ESList builder needs function protype!");

  if (!FuncProto->getNumExceptions())
    return;

  llvm::Type *esListTy = getESListType(CGM);

  llvm::SmallVector<llvm::Constant *, 2> fields;
  int numExceptions = FuncProto->getNumExceptions();

  fields.push_back(llvm::ConstantInt::get(Int32Ty, numExceptions));

  // If function doesn`t have throw specs, then we do not need build eslist
  // handler array.
  if (!numExceptions) {
    llvm::StructType *esListStructTy = cast<llvm::StructType>(esListTy);
    llvm::Type *handlerPtrTy = esListStructTy->getStructElementType(1);

    fields.push_back(llvm::UndefValue::get(handlerPtrTy));

    llvm::Constant *init = 
      llvm::ConstantStruct::get(esListStructTy, fields);

    EHState.ESTypeList = new llvm::GlobalVariable(CGM.getModule(), 
                                                  init->getType(), false,
                                  llvm::GlobalValue::WeakAnyLinkage, init, "");
    return;
  }

  llvm::SmallVector<llvm::Constant *, 2> expectedExceptionsHandlers;
  llvm::Type *handlerTy = getHandlerType(CGM);

  for (FunctionProtoType::exception_iterator I = FuncProto->exception_begin(),
       E = FuncProto->exception_end(); I != E; ++I) {
    QualType excType = *I;
    expectedExceptionsHandlers.push_back(getTypeHandlerForESList(CGM,
                                                                 handlerTy,
                                                                 excType));
  }

  llvm::ArrayType *arrTy = llvm::ArrayType::get(handlerTy, numExceptions);
  llvm::Constant *exceptionArray = 
    llvm::ConstantArray::get(arrTy, expectedExceptionsHandlers);
  
  llvm::GlobalValue *handlers = 
    new llvm::GlobalVariable(CGM.getModule(), exceptionArray->getType(), false,
                        llvm::GlobalValue::WeakAnyLinkage, exceptionArray, "");

  llvm::Constant *idxs[] = { 
    llvm::ConstantInt::get(Int32Ty, 0),
    llvm::ConstantInt::get(Int32Ty, 0) 
  };

  fields.push_back(llvm::ConstantExpr::getGetElementPtr(handlers, idxs));

  llvm::Constant *init = 
    llvm::ConstantStruct::get(cast<llvm::StructType>(esListTy), fields);
  
  EHState.ESTypeList = new llvm::GlobalVariable(CGM.getModule(), 
                                                init->getType(), false,
                                  llvm::GlobalValue::WeakAnyLinkage, init, "");
}

// r4start
llvm::GlobalValue *CodeGenFunction::EmitUnwindTable() {
  const FunctionDecl *funcDecl = cast_or_null<FunctionDecl>(CurFuncDecl);
  assert(funcDecl && "Unwind table is generating only for functions!");

  llvm::SmallString<256> tableName;
  llvm::raw_svector_ostream stream(tableName);

  CGM.getCXXABI().getMangleContext().
    getMsExtensions()->mangleEHUnwindTable(funcDecl, stream);
  stream.flush();

  StringRef mangledUnwindTableName(tableName);

  llvm::StructType *entryTy = getUnwindMapEntryTy(CGM);

  llvm::SmallVector<llvm::Constant *, 2> fields;
  llvm::SmallVector<llvm::Constant *, 4> entries;

  assert(!EHState.GlobalUnwindTable.empty() && "Unwind table is empty!");

  llvm::BasicBlock *oldBB = Builder.GetInsertBlock();
  llvm::Constant *nullVal = llvm::ConstantPointerNull::get(CGM.VoidPtrTy);

  
  // We must skip first element, because it is initial state
  // and it is not valuable for unwind table.
  for (MSEHState::UnwindTableTy::iterator 
       I = EHState.GlobalUnwindTable.begin(),
       E = EHState.GlobalUnwindTable.end(); I != E; ++I) {
    if (!I->Funclet) {
      // creating unwind table entry
      fields.push_back(llvm::ConstantInt::get(Int32Ty, I->ToState));

      fields.push_back(llvm::ConstantExpr::getBitCast(nullVal, 
                                                   entryTy->getElementType(1)));

      entries.push_back(llvm::ConstantStruct::get(entryTy, fields));
      fields.clear();
      continue;
    }

    // creating unwind table entry
    fields.push_back(llvm::ConstantInt::get(Int32Ty, I->ToState));
    fields.push_back(llvm::BlockAddress::get(CurFn, I->Funclet));

    entries.push_back(llvm::ConstantStruct::get(entryTy, fields));
    fields.clear();
  }

  Builder.SetInsertPoint(oldBB);

  llvm::ArrayType *tableTy = llvm::ArrayType::get(entryTy, entries.size());
  llvm::Constant *init = llvm::ConstantArray::get(tableTy, entries);

  return new llvm::GlobalVariable(CGM.getModule(), init->getType(), false,
                                  llvm::GlobalValue::WeakAnyLinkage, init,
                                  mangledUnwindTableName);
}

// r4start
void CodeGenFunction::MSEHState::InitOffsetInCatchHandlers() {
  llvm::BasicBlock *currentIP = CGF.Builder.GetInsertBlock();
  llvm::BasicBlock *sehInitBlock = 
    llvm::BasicBlock::Create(CGF.CGM.getLLVMContext(), "seh.init.block",
                             CGF.CurFn);
  llvm::BasicBlock *entryBlock =  &CGF.CurFn->getEntryBlock();

  CGF.Builder.SetInsertPoint(entryBlock);
  
  for (llvm::BasicBlock::iterator I = entryBlock->begin(),
       E = entryBlock->end(); I != E; ++I) {
    if (dyn_cast<llvm::AllocaInst>(I)) {
      continue;
    }

    llvm::BasicBlock *afterSEHInit = 
      entryBlock->splitBasicBlock(I, "after.seh.init");
    entryBlock->getTerminator()->eraseFromParent();
    SaveStackPointer();
    CGF.Builder.CreateBr(sehInitBlock);
    entryBlock = afterSEHInit;
    break;
  }

  CGF.Builder.SetInsertPoint(sehInitBlock);

  llvm::Function *frameAddr = 
      CGF.CGM.getIntrinsic(llvm::Intrinsic::frameaddress);
  llvm::Value *ebp = 
    CGF.Builder.CreateCall(frameAddr, 
                           llvm::ConstantInt::get(CGF.Int32Ty, 0));
  ebp = CGF.Builder.CreatePtrToInt(ebp, CGF.Int32Ty);
  
  llvm::Constant *zero = llvm::ConstantInt::get(CGF.Int32Ty, 0);
  llvm::Constant *fieldIdx = llvm::ConstantInt::get(CGF.Int32Ty, 2);

  for (HandlersArray::iterator I = CatchHandlers.begin(), 
       E = CatchHandlers.end(); I != E; ++I) {
    if (!I->ExceptionObject)
      continue;

    llvm::Value *objAddr = CGF.GetAddrOfLocalVar(I->ExceptionObject);
    objAddr = CGF.Builder.CreatePtrToInt(objAddr, CGF.Int32Ty);
    objAddr = CGF.Builder.CreateSub(objAddr, ebp);

    llvm::Value *idxs[] = {
      zero,
      llvm::ConstantInt::get(CGF.Int32Ty, I->Index),
      fieldIdx
    };

    llvm::Value *ebpBasedOffsetField = 
      CGF.Builder.CreateGEP(I->Handlers, idxs);
    ebpBasedOffsetField = 
      CGF.Builder.CreateBitCast(ebpBasedOffsetField, 
                                CGF.Int32Ty->getPointerTo());
    CGF.Builder.CreateStore(objAddr, ebpBasedOffsetField);
  }

  sehInitBlock->moveBefore(entryBlock);

  CGF.Builder.CreateBr(entryBlock);
  CGF.Builder.SetInsertPoint(currentIP);
}

namespace {

struct StartOfNewCatchHandlers {
  typedef CodeGenFunction::MSEHState::CatchHandler CatchHandler;
  typedef CodeGenFunction::MSEHState::UnwindTableTy::iterator TryEntry;

  StartOfNewCatchHandlers(TryEntry Parent) : InnermostTry(Parent) {}

  bool operator() (const CatchHandler &Handler) {
    return Handler.Handlers == 0 &&
           Handler.ParentTry == InnermostTry;
  }

  private:
    TryEntry InnermostTry;
};

struct InitNewCatchHandlersHandler {
  typedef CodeGenFunction::MSEHState::CatchHandler CatchHandler;
  llvm::GlobalVariable *GV;
  InitNewCatchHandlersHandler(llvm::GlobalVariable *G) : GV(G) {}

  void operator() (CatchHandler &Handler) {
    Handler.Handlers = GV;
  }
};

}

// r4start
void CodeGenFunction::GenerateTryBlockTableEntry() {
  llvm::Type *handlerTy = (*EHState.TryHandlers.back().begin())->getType();

  llvm::SmallVector<llvm::Constant *, 5> fields;

  MSEHState::UnwindTableTy::iterator firstState = 
    *EHState.LocalUnwindTable.back().begin();
  
  llvm::Constant *tryLow = 
    cast<llvm::Constant>(firstState->Store->getOperand(0));
  fields.push_back(tryLow);

  MSEHState::UnwindTableTy::reverse_iterator lastEntry = 
    EHState.GlobalUnwindTable.rbegin();
 
  while (lastEntry != EHState.GlobalUnwindTable.rend()) {
    if (lastEntry->RestoreKind == RestoreOpInfo::CatchRestore &&
        lastEntry->StoreInstTryLevel == firstState->StoreInstTryLevel &&
        lastEntry->TryNumber == firstState->TryNumber) {
      break;
    }
    ++lastEntry;
  }
  
  assert (lastEntry != EHState.GlobalUnwindTable.rend() &&
          "Can not find last entry for this try block!");
 
  llvm::Constant *catchHigh = 
    llvm::ConstantInt::get(Int32Ty, lastEntry->StoreIndex);

  ++lastEntry;
  
  assert (lastEntry != EHState.GlobalUnwindTable.rend() &&
          "Can not find catch entry for this try block!");

  llvm::Constant *tryHigh = 
    llvm::ConstantInt::get(Int32Ty, lastEntry->StoreIndex);
  
  fields.push_back(tryHigh);
  fields.push_back(catchHigh);

  fields.push_back(llvm::ConstantInt::get(Int32Ty,
                                          EHState.TryHandlers.back().size()));

  llvm::ArrayType *arrayOfHandlersTy = 
    llvm::ArrayType::get(handlerTy, EHState.TryHandlers.back().size());
  llvm::Constant *handlersArray = 
               llvm::ConstantArray::get(arrayOfHandlersTy,
                                        EHState.TryHandlers.back());

  llvm::SmallString<256> tableName;
  llvm::raw_svector_ostream stream(tableName);

  const FunctionDecl *funcDecl = cast_or_null<FunctionDecl>(CurFuncDecl);
  assert(funcDecl && "Unwind table is generating only for functions!");

  CGM.getCXXABI().getMangleContext().
    getMsExtensions()->mangleEHCatchHandlersArray(funcDecl, 
                                                  EHState.EHManglingCounter++,
                                                  stream);
  stream.flush();

  StringRef mangledName(tableName);

  llvm::GlobalVariable *globalHandlers = 
    new llvm::GlobalVariable(CGM.getModule(), handlersArray->getType(), false,
                             llvm::GlobalValue::WeakAnyLinkage, handlersArray,
                             mangledName);

  MSEHState::HandlersArray::iterator
    newHandlers = std::find_if(EHState.CatchHandlers.begin(),
                               EHState.CatchHandlers.end(),
                               StartOfNewCatchHandlers(firstState));

  assert(newHandlers != EHState.CatchHandlers.end() &&
         "Try block must have catch handlers!");

  MSEHState::HandlersArray::reverse_iterator
    endOfNewHandlers = std::find_if(EHState.CatchHandlers.rbegin(),
                                    EHState.CatchHandlers.rend(),
                                    StartOfNewCatchHandlers(firstState));
  
  assert(endOfNewHandlers != EHState.CatchHandlers.rend() &&
         "Can not find last of catch handlers for current try!");

  std::for_each(endOfNewHandlers,
                MSEHState::HandlersArray::reverse_iterator(newHandlers),
                InitNewCatchHandlersHandler(globalHandlers));

  llvm::Constant *addrOfGlobalhandlers = 
    llvm::ConstantExpr::getBitCast(globalHandlers, 
                                   getHandlerType(CGM)->getPointerTo());
  fields.push_back(addrOfGlobalhandlers);

  llvm::StructType *entryTy = getTryBlockMapEntryTy(CGM);
  llvm::Constant *init = llvm::ConstantStruct::get(entryTy, fields);
  
  EHState.TryBlockTableEntries.push_back(init);
  
  // After all we must clear all entries,
  // because it can be nested try.
  EHState.TryHandlers.pop_back();
}

// r4start
llvm::GlobalValue *CodeGenFunction::EmitTryBlockTable() {
  const FunctionDecl *funcDecl = cast_or_null<FunctionDecl>(CurFuncDecl);
  assert(funcDecl && "Try block table is generating only for functions!");

  llvm::SmallString<256> tableName;
  llvm::raw_svector_ostream stream(tableName);

  CGM.getCXXABI().getMangleContext().
    getMsExtensions()->mangleEHTryBlockTable(funcDecl, stream);
  stream.flush();

  StringRef mangledTryBlockTableName(tableName);

  assert(!EHState.TryBlockTableEntries.empty() && 
                                                "Try block can not be empty!");

  llvm::ArrayType *tableTy = 
    llvm::ArrayType::get((*EHState.TryBlockTableEntries.begin())->getType(),
                         EHState.TryBlockTableEntries.size());
  llvm::Constant *init = 
    llvm::ConstantArray::get(tableTy, EHState.TryBlockTableEntries);

  return new llvm::GlobalVariable(CGM.getModule(), init->getType(), false,
                                  llvm::GlobalValue::WeakAnyLinkage, init,
                                  mangledTryBlockTableName);
}

// r4start
llvm::GlobalValue *CodeGenFunction::EmitMSFuncInfo() {
  llvm::GlobalValue *unwindTable = 0;
  llvm::GlobalValue *tryBlocksTable = 0;

  // If function doesn`t have try-block,
  // then funcinfo is not necessary.
  if (!EHState.TryBlockTableEntries.empty()) {
    tryBlocksTable = EmitTryBlockTable();
  }

  if (!EHState.GlobalUnwindTable.empty()) {
    unwindTable = EmitUnwindTable();
  }

  const FunctionDecl *function = 
    cast_or_null<FunctionDecl>(CurGD.getDecl());
  assert(function && "Func info can be generated only for functions!");

  llvm::StructType *ehFuncInfoTy;
  if (llvm::Type *funcInfo = CGM.getModule().getTypeByName("ehfuncinfo")) {
    ehFuncInfoTy = cast<llvm::StructType>(funcInfo);
  } else {
    ehFuncInfoTy = generateEHFuncInfoType(CGM);
  }
  assert(ehFuncInfoTy && "Problems with __ehfuncinfo type!");

  llvm::SmallVector<llvm::Constant *, 9> initializerFields;

  // magic number
  initializerFields.push_back(llvm::ConstantInt::get(Int32Ty, 0x19930522));

  llvm::Constant *zeroVal = llvm::ConstantInt::get(Int32Ty, 0);

  // number of entries in unwind table
  initializerFields.push_back(
    llvm::ConstantInt::get(Int32Ty, EHState.GlobalUnwindTable.size()));

  llvm::Constant *idxList[] = { zeroVal, zeroVal };

  // pUnwindTable
  if (unwindTable) {
    initializerFields.push_back(
      llvm::ConstantExpr::getInBoundsGetElementPtr(unwindTable, idxList));
  } else {
    initializerFields.push_back(
     llvm::ConstantPointerNull::get(getUnwindMapEntryTy(CGM)->getPointerTo()));
  }
  // number of try blocks in the function
  initializerFields.push_back(llvm::ConstantInt::get(Int32Ty,
                                          EHState.TryBlockTableEntries.size()));
  // pTryBlockTable
  if (tryBlocksTable) {
    initializerFields.push_back(
      llvm::ConstantExpr::getInBoundsGetElementPtr(tryBlocksTable, idxList));
  } else {
    initializerFields.push_back(
      llvm::ConstantPointerNull::get(
        getTryBlockMapEntryTy(CGM)->getPointerTo()));
  }

  // not used on x86
  initializerFields.push_back(zeroVal);

  // not used on x86
  initializerFields.push_back(llvm::ConstantPointerNull::get(VoidPtrTy));

  // ESTypeList
  if (!EHState.ESTypeList) {
    initializerFields.push_back(
      llvm::ConstantPointerNull::get(getESListType(CGM)->getPointerTo()));
  } else {
    initializerFields.push_back(EHState.ESTypeList);
  }

  // EHFlags
  // Bit 0 was setted, because at now driver doesn`t support MS
  // specific keys and our code doesn`t support SEH.
  // So we setted this bit according /EHs cl flag.
  // In future, when clang will support cl options, we can
  // get this option from upper code.
  initializerFields.push_back(llvm::ConstantInt::get(Int32Ty, 1));

  llvm::Constant *init = 
    llvm::ConstantStruct::get(ehFuncInfoTy, initializerFields);

  llvm::SmallString<256> structName;
  llvm::raw_svector_ostream out(structName);

  MSMangleContextExtensions *mangler = 
    CGM.getCXXABI().getMangleContext().getMsExtensions();

  mangler->mangleEHFuncInfo(function, out);

  out.flush();
  StringRef ehName(structName);

  return new llvm::GlobalVariable(CGM.getModule(), init->getType(), false,
                              llvm::GlobalValue::WeakAnyLinkage, init, ehName);
}

// r4start
void CodeGenFunction::EmitEHInformation() {
  if (!EHState.EHHandler) {
    return;
  }
  

  llvm::GlobalValue *ehFuncInfo = EmitMSFuncInfo();
  if (!ehFuncInfo) {
    return;
  }

  llvm::BasicBlock *old = Builder.GetInsertBlock();

  Builder.SetInsertPoint(EHState.EHHandler);

  llvm::Function *frameHandlerFunc = 
    getMSFrameHandlerFunction(*this, ehFuncInfo->getType());

  llvm::CallInst *call = Builder.CreateCall(frameHandlerFunc, ehFuncInfo);
  call->setTailCall();
  call->setDoesNotReturn();

  Builder.CreateUnreachable();
  Builder.SetInsertPoint(old);

  EHState.InitOffsetInCatchHandlers();
}

// r4start
void CodeGenFunction::GenerateCatchHandler(const QualType &CaughtType,
                                           llvm::BlockAddress *HandlerAddress,
                                           int HandlerIdx) {
  // 0x01: const, 0x02: volatile, 0x08: reference, 0x40 ellipsis
  int handlerAdjectives = 0;
  QualType nonQualifiedType;
  llvm::Constant *typeDescriptor;

  if (CaughtType.getAsOpaquePtr()) {
    if (CaughtType.isConstQualified()) {
      handlerAdjectives |= 1;
    }

    if (CaughtType.isVolatileQualified()) {
      handlerAdjectives |= 2;
    }

    if (CaughtType->isReferenceType()) {
      handlerAdjectives |= 8;
    }

  nonQualifiedType  = 
    CaughtType.getNonReferenceType().getUnqualifiedType();
  typeDescriptor = 
    CGM.GetAddrOfMSTypeDescriptor(nonQualifiedType);
  typeDescriptor = llvm::ConstantExpr::getBitCast(typeDescriptor,
                                          CGM.GetDescriptorPtrType(Int8PtrTy));
  } else {
    // case for catch (...)
    handlerAdjectives = 0x40;

    llvm::PointerType *descrTy = 
      cast<llvm::PointerType>(CGM.GetDescriptorPtrType(Int8PtrTy));
    typeDescriptor = llvm::ConstantPointerNull::get(descrTy);
  }

  llvm::SmallVector<llvm::Constant *, 4> fields;

  // adjectives
  fields.push_back(llvm::ConstantInt::get(Int32Ty, handlerAdjectives));

  // pTypeDescr
  fields.push_back(typeDescriptor);

  // dispatch obj
  fields.push_back(llvm::ConstantInt::get(Int32Ty, 0));

  // address of handler
  fields.push_back(llvm::ConstantExpr::getBitCast(HandlerAddress, VoidPtrTy));

  llvm::Constant *handler = 
    llvm::ConstantStruct::get(getHandlerType(CGM), fields);
  EHState.TryHandlers.back().push_back(handler);
  
  EHState.CatchHandlers.push_back(
    MSEHState::CatchHandler(HandlerIdx, 
                            *EHState.LocalUnwindTable.back().begin()));
}
