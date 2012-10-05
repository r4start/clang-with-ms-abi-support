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
void CodeGenFunction::MSEHState::InitMSTryState() {
  if (!MSTryState) {
    MSTryState = CGF.CreateTempAlloca(CGF.Int32Ty, "try.id");
    CreateStateStore(-1);
  }
}

// r4start
void CodeGenFunction::MSEHState::SetMSTryState() {
  size_t state = GlobalUnwindTable.size();
  if (!state) {
    GlobalUnwindTable.push_back(-1);
  } else {
    GlobalUnwindTable.push_back(GlobalUnwindTable.back().StoreValue);
  }
  MsUnwindInfo &backRef = GlobalUnwindTable.back();
  
  LocalUnwindTable.back().push_back(--GlobalUnwindTable.end());

  backRef.IsUsed = true;

  backRef.Store = CreateStateStore(state);
  backRef.StoreValue = state;
  backRef.StoreInstTryLevel = TryLevel;
  backRef.TryNumber = TryNumber;
  backRef.StoreIndex = StoreIndex++;
}

// r4start
llvm::StoreInst *CodeGenFunction::MSEHState::CreateStateStore(uint32_t State) {
  return CGF.Builder.CreateStore(llvm::ConstantInt::get(CGF.Int32Ty, State),
                                 MSTryState);
}

llvm::StoreInst *CodeGenFunction::MSEHState::CreateStateStore(llvm::Value *State) {
  return CGF.Builder.CreateStore(State, MSTryState);
}

// r4start
llvm::StoreInst *
CodeGenFunction::MSEHState::CreateStateStoreWithoutEmit(llvm::Value *State) {
  return new llvm::StoreInst(State, MSTryState);
}

llvm::Instruction *
CodeGenFunction::MSEHState::AddCatchEntryInUnwindTable(size_t Index,
                                                       llvm::Value *State) {
  GlobalUnwindTable.push_back(Index);
  GlobalUnwindTable.back().StoreValue = Index;

  llvm::StoreInst *store = CreateStateStoreWithoutEmit(State);

  GlobalUnwindTable.back().Store = store;
  
  // TryNumber must be right because it necessary for GenerateTryBlockEntry.
  // If TryNumber is wrong then GenerateTryBlockEntry can not find this entry
  // in global unwind table.
  if (!LastEntries.empty()) {
    GlobalUnwindTable.back().TryNumber = 
      LastEntries.back()->TryNumber + 1;
  } else {
    GlobalUnwindTable.back().TryNumber = 
      (*(LocalUnwindTable.back().begin()))->TryNumber;
  }

  GlobalUnwindTable.back().StoreInstTryLevel = TryLevel;
  GlobalUnwindTable.back().StoreIndex = StoreIndex++;
  GlobalUnwindTable.back().RestoreKind = RestoreOpInfo::CatchRestore;

  LastEntries.push_back(--GlobalUnwindTable.end());
  return store;
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
static llvm::Constant *getNullPointer(llvm::Type *Type) {
  return llvm::ConstantPointerNull::get(cast<llvm::PointerType>(Type)); 
}

// r4start
static llvm::Constant *getMSThrowFn(CodeGenFunction &CGF, 
                                    llvm::Type *ThrowInfoPtrTy) {
  llvm::Type *Args[2] = { CGF.Int8PtrTy, ThrowInfoPtrTy };
  llvm::FunctionType *FTy =
    llvm::FunctionType::get(CGF.VoidTy, Args, /*IsVarArgs=*/false);

  return CGF.CGM.CreateRuntimeFunction(FTy, "_CxxThrowException@8");
}

// r4start
static llvm::Function *getMSFrameHandlerFunction(CodeGenFunction &CGF, 
                                                 llvm::Type *EHFuncInfoTy) {
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
  return new llvm::GlobalVariable(CGM.getModule(), init->getType(), true,
                                  llvm::GlobalValue::ExternalLinkage, 
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
  typedef llvm::SmallVector<llvm::Type *, 8> CTATypes;
  typedef llvm::SmallVector<QualType, 8> CatchablesContainerTy;

  CatchablesContainerTy catchableTypes;
  if (const CXXRecordDecl *throwDecl = ThrowType->getAsCXXRecordDecl()) {
    discoverAllCatchabletypes<CatchablesContainerTy>(throwDecl, catchableTypes);
  } else {
    catchableTypes.push_back(ThrowType);
  }

  InitContainerTy init;
  init.push_back(llvm::ConstantInt::get(CGM.Int32Ty, numberOfBases));
  std::for_each(catchableTypes.begin(), catchableTypes.end(), 
                CatchableGenerator<InitContainerTy>(CGM, init));
  
  CTATypes fieldTypes;
  for (InitContainerTy::iterator I = init.begin(), E = init.end();
       I != E; ++I) {
    fieldTypes.push_back((*I)->getType());
  }

  llvm::StructType *ctaType = 
    llvm::StructType::get(CGM.getLLVMContext(), fieldTypes);

  llvm::Constant *cta = llvm::ConstantStruct::get(ctaType, init);

  return
    new llvm::GlobalVariable(CGM.getModule(), cta->getType(), true,
                             llvm::GlobalValue::ExternalLinkage,
                             cta, mangledName);
}

// r4start
static llvm::Constant *generateThrowInfoInit(CodeGenFunction &CGF, 
                                             QualType ThrowTy,
                                             llvm::StructType *ThrowInfoTy) {
  const CXXRecordDecl *throwTypeDecl = ThrowTy->getAsCXXRecordDecl();
  llvm::GlobalValue *dtorPtr = 0;
  if (throwTypeDecl) {
    const CXXDestructorDecl *dtorDecl = throwTypeDecl->getDestructor();
    dtorPtr = CGF.CGM.GetAddrOfCXXDestructor(dtorDecl, Dtor_Base);
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

  return new llvm::GlobalVariable(CGF.CGM.getModule(), throwInfoTy, true,
                                  llvm::GlobalValue::ExternalLinkage,
                                  throwInfoInit, throwInfo);
}

// r4start
void CodeGenFunction::EmitMSCXXThrowExpr(const CXXThrowExpr *E) {
  if (!E->getSubExpr()) {
    assert(false && "I do not know how ms do throw without sub expression!");
    return;
  }

  QualType ThrowType = E->getSubExpr()->getType();

  uint64_t ThrowTypeSize = 
    getContext().getTypeSizeInChars(ThrowType).getQuantity();

  llvm::Type *throwTy = CGM.getTypes().ConvertType(ThrowType);

  llvm::AllocaInst *ThrowObj = Builder.CreateAlloca(throwTy, 0,
                                                    "throw.object");

  // If need emits ctor call.
  EmitAnyExprToMem(E->getSubExpr(), ThrowObj,
                   E->getSubExpr()->getType().getQualifiers(), true);

  llvm::Value *CXXThrowExParam = Builder.CreateBitCast(ThrowObj, CGM.Int8PtrTy);

  llvm::GlobalVariable *ThrowInfo = getOrGenerateThrowInfo(*this, ThrowType);

  llvm::Value *Params[] = { CXXThrowExParam, ThrowInfo };
                        
  Builder.CreateCall(getMSThrowFn(*this, ThrowInfo->getType()), Params);
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
static llvm::Type *getHandlerType(CodeGenModule &CGM) {
  if (llvm::Type *handlerTy = CGM.getModule().getTypeByName("handler.type")) {
    return handlerTy;
  }
  SmallVector<llvm::Type *, 4> fields;

  fields.push_back(CGM.Int32Ty);

  // Type descriptor.
  fields.push_back(CGM.GetDescriptorPtrType(CGM.Int8PtrTy));

  fields.push_back(CGM.Int32Ty);

  fields.push_back(CGM.VoidPtrTy);

  return llvm::StructType::create(CGM.getLLVMContext(), fields, "handler.type");
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
static llvm::Function *getEHHandler(CodeGenFunction &CGF) {
  const FunctionDecl *func = cast_or_null<FunctionDecl>(CGF.CurFuncDecl);
  assert(func && "EH handler can be generated only for function or method!");

  llvm::SmallString<256> nameBuffer;
  llvm::raw_svector_ostream nameStream(nameBuffer);

  CGF.CGM.getCXXABI().getMangleContext().getMsExtensions()->
    mangleEHHandlerFunction(func, nameStream);

  nameStream.flush();
  StringRef mangledName(nameBuffer);

  llvm::Function *F = CGF.CGM.getModule().getFunction(mangledName);
  if (F) {
    return F;
  }

  llvm::FunctionType *FTy = llvm::FunctionType::get(CGF.VoidTy, false);

  F = llvm::Function::Create(FTy,
        llvm::GlobalValue::ExternalLinkage, mangledName,
        &CGF.CGM.getModule());
  return F;
}

// r4start
void CodeGenFunction::EmitESTypeList(const FunctionProtoType *FuncProto) {
  assert(FuncProto && "ESList builder needs function protype!");
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
                                                  init->getType(), true,
                                  llvm::GlobalValue::InternalLinkage, init, "");
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
    new llvm::GlobalVariable(CGM.getModule(), exceptionArray->getType(), true,
                        llvm::GlobalValue::InternalLinkage, exceptionArray, "");

  llvm::Constant *idxs[] = { 
    llvm::ConstantInt::get(Int32Ty, 0),
    llvm::ConstantInt::get(Int32Ty, 0) 
  };

  fields.push_back(llvm::ConstantExpr::getGetElementPtr(handlers, idxs));

  llvm::Constant *init = 
    llvm::ConstantStruct::get(cast<llvm::StructType>(esListTy), fields);
  
  EHState.ESTypeList = new llvm::GlobalVariable(CGM.getModule(), 
                                                init->getType(), true,
                                  llvm::GlobalValue::InternalLinkage, init, "");
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

  llvm::BasicBlock *prevBlock = 0;

  // We must skip first element, because it is initial state
  // and it is not valuable for unwind table.
  for (MSEHState::UnwindTableTy::iterator 
       I = EHState.GlobalUnwindTable.begin(),
       E = EHState.GlobalUnwindTable.end(); I != E; ++I) {
    if (!I->ReleaseFunc) {
      // creating unwind table entry
      fields.push_back(llvm::ConstantInt::get(Int32Ty, I->ToState));

      fields.push_back(llvm::ConstantExpr::getBitCast(nullVal, 
                                                   entryTy->getElementType(1)));

      entries.push_back(llvm::ConstantStruct::get(entryTy, fields));
      fields.clear();
      continue;
    }

    // creating funclet code
    llvm::SmallString<256> funcletName;
    llvm::raw_svector_ostream stream(funcletName);

    CGM.getCXXABI().getMangleContext().
      getMsExtensions()->mangleEHUnwindFunclet(funcDecl,
                                                EHState.EHManglingCounter,
                                                stream);

    EHState.EHManglingCounter++;

    stream.flush();
    StringRef funcletNameRef(funcletName);

    llvm::BasicBlock *funclet = 
      llvm::BasicBlock::Create(CGM.getLLVMContext(), funcletNameRef, CurFn);

    if (EHState.LastCatchHandler) {
      // At first remove unreachable instruction from the end.
      EHState.LastCatchHandler->getTerminator()->eraseFromParent();
      Builder.SetInsertPoint(EHState.LastCatchHandler);
      Builder.CreateBr(funclet);
      EHState.LastCatchHandler = 0;
    }

    if (prevBlock) {
      Builder.CreateBr(funclet);
    }
    prevBlock = funclet;

    Builder.SetInsertPoint(funclet);

    llvm::CallInst *call = 
      Builder.CreateCall(I->ReleaseFunc, I->ThisPtr);

    
    // creating unwind table entry
    fields.push_back(llvm::ConstantInt::get(Int32Ty, I->ToState));
    fields.push_back(llvm::BlockAddress::get(CurFn, funclet));

    entries.push_back(llvm::ConstantStruct::get(entryTy, fields));
    fields.clear();
  }

  if (prevBlock) {
    // last funclet jumps to ehhandler
    llvm::Function *ehHandler = getEHHandler(*this);
    llvm::CallInst *call = Builder.CreateCall(ehHandler);
    call->setDoesNotReturn();
    Builder.CreateUnreachable();
  }

  Builder.SetInsertPoint(oldBB);

  llvm::ArrayType *tableTy = llvm::ArrayType::get(entryTy, entries.size());
  llvm::Constant *init = llvm::ConstantArray::get(tableTy, entries);

  return new llvm::GlobalVariable(CGM.getModule(), init->getType(), true,
                                  llvm::GlobalValue::InternalLinkage, init,
                                  mangledUnwindTableName);
}

// r4start
void CodeGenFunction::GenerateTryBlockTableEntry() {
  llvm::Type *handlerTy = (*EHState.TryHandlers.begin())->getType();

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

  fields.push_back(llvm::ConstantInt::get(Int32Ty, EHState.TryHandlers.size()));

  llvm::ArrayType *arrayOfHandlersTy = 
    llvm::ArrayType::get(handlerTy, EHState.TryHandlers.size());
  llvm::Constant *handlersArray = 
               llvm::ConstantArray::get(arrayOfHandlersTy, EHState.TryHandlers);

  // After all we must clear all entries,
  // because it can be nested try.
  EHState.TryHandlers.clear();

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
    new llvm::GlobalVariable(CGM.getModule(), handlersArray->getType(), true,
                             llvm::GlobalValue::InternalLinkage, handlersArray,
                             mangledName);

  llvm::Constant *addrOfGlobalhandlers = 
    llvm::ConstantExpr::getBitCast(globalHandlers, 
                                   getHandlerType(CGM)->getPointerTo());
  fields.push_back(addrOfGlobalhandlers);

  llvm::StructType *entryTy = getTryBlockMapEntryTy(CGM);
  llvm::Constant *init = llvm::ConstantStruct::get(entryTy, fields);
  
  EHState.TryBlockTableEntries.push_back(init);
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

  return new llvm::GlobalVariable(CGM.getModule(), init->getType(), true,
                                  llvm::GlobalValue::InternalLinkage, init,
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
  } else {
    return 0;
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

  // number of entries in unwind table
  initializerFields.push_back(
    llvm::ConstantInt::get(Int32Ty, EHState.GlobalUnwindTable.size()));

  llvm::Constant *zeroVal = llvm::ConstantInt::get(Int32Ty, 0);
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
  initializerFields.push_back(llvm::UndefValue::get(VoidPtrTy));

  // ESTypeList
  initializerFields.push_back(EHState.ESTypeList);

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

  return new llvm::GlobalVariable(CGM.getModule(), init->getType(), true,
                              llvm::GlobalValue::InternalLinkage, init, ehName);
}

// r4start
void CodeGenFunction::EmitEHInformation() {
  llvm::GlobalValue *ehFuncInfo = EmitMSFuncInfo();
  if (!ehFuncInfo) {
    return;
  }

  llvm::Function *ehHandler = getEHHandler(*this);

  CodeGenFunction cgf(CGM);

  llvm::Function *frameHandlerFunc = 
    getMSFrameHandlerFunction(cgf, ehFuncInfo->getType());

  llvm::BasicBlock *BB = 
    llvm::BasicBlock::Create(cgf.CGM.getLLVMContext(), "entry", ehHandler);
  cgf.Builder.SetInsertPoint(BB);

  llvm::CallInst *call = cgf.Builder.CreateCall(frameHandlerFunc, ehFuncInfo);
  call->setTailCall();
  call->setDoesNotReturn();

  cgf.Builder.CreateUnreachable();
}

// r4start
void CodeGenFunction::GenerateCatchHandler(QualType &CaughtType,
                                           llvm::Type *HandlerTy,
                                           llvm::BlockAddress *HandlerAddress) {
  // 0x01: const, 0x02: volatile, 0x08: reference
  int handlerAdjectives = 0;
  if (CaughtType.isConstQualified()) {
    handlerAdjectives |= 1;
  }

  if (CaughtType.isVolatileQualified()) {
    handlerAdjectives |= 2;
  }

  if (CaughtType->isReferenceType()) {
    handlerAdjectives |= 8;
  }

  CaughtType = CaughtType.getNonReferenceType().getUnqualifiedType();
  llvm::Constant *TypeDescriptor = CGM.GetAddrOfMSTypeDescriptor(CaughtType);

  llvm::SmallVector<llvm::Constant *, 4> fields;

  // adjectives
  fields.push_back(llvm::ConstantInt::get(Int32Ty, handlerAdjectives));

  // pTypeDescr
  fields.push_back(llvm::ConstantExpr::getBitCast(TypeDescriptor,
    CGM.GetDescriptorPtrType(Int8PtrTy)));

  // dispatch obj
  // TODO: generate right epb offset!
  fields.push_back(llvm::ConstantInt::get(Int32Ty, 0));

  // address of handler
  fields.push_back(llvm::ConstantExpr::getBitCast(HandlerAddress, VoidPtrTy));

  llvm::Constant *handler = 
    llvm::ConstantStruct::get(cast<llvm::StructType>(HandlerTy), fields);
  EHState.TryHandlers.push_back(handler);
}

// r4start
static MsUnwindInfo CreateCatchRestore(llvm::StoreInst *Store, int Index,
                                       int Value, int StoreTryLevel,
                                       int TopTryNumber) {
// MsUnwindInfo(int State, llvm::Value *This, llvm::Value *RF, 
//              int StoreVal = -2, bool Used = false, 
//              RestoreOpInfo::RestoreOpKind RestoreOp = RestoreOpInfo::Undef,
//              llvm::StoreInst *StoreInstruction = 0, int StoreTryLevel = -1,
//              int TopLevelTryNumber = -1, int Index = -1)
  return MsUnwindInfo(-1, 0, 0, Value, true, RestoreOpInfo::CatchRestore,
                      Store, StoreTryLevel, TopTryNumber, Index);
}

// r4start
static void insertCatchRet(CodeGenFunction &CGF) {
  llvm::FunctionType *fty = llvm::FunctionType::get(CGF.VoidTy, false);
  llvm::InlineAsm *ia = llvm::InlineAsm::get(fty, "ret", "", false);

  llvm::CallInst *result = CGF.Builder.CreateCall(ia);
  result->addAttribute(~0, llvm::Attribute::NoUnwind);
  result->addAttribute(~0, llvm::Attribute::IANSDialect);
}

static llvm::Constant *getFakePersonality(CodeGenFunction &CGF) {
  llvm::Constant *Fn =
    CGF.CGM.CreateRuntimeFunction(
                              llvm::FunctionType::get(CGF.CGM.Int32Ty, true),
                              "ms_fake_personality");
  return Fn;
}

// This is need to avoid deleting catch handler from function.
// We generate a chain of br instructions from lpad to last of catch handlers.
// BrFrom - previous handler or lpad.
// BrTo - current catch handler.
static void doHandlerReachable(CodeGen::CGBuilderTy &Builder,
                               llvm::BasicBlock * &BrFrom,
                               llvm::BasicBlock * &BrTo) {
  Builder.SetInsertPoint(BrFrom);
  Builder.CreateBr(BrTo);
  Builder.SetInsertPoint(BrTo);
  BrFrom = BrTo;
}

// r4start
void CodeGenFunction::ExitMSCXXTryStmt(const CXXTryStmt &S) {
  unsigned NumHandlers = S.getNumHandlers();
  
  EHStack.popCatch();
  
  const FunctionDecl *fd = cast_or_null<FunctionDecl>(CurFuncDecl);
  assert(fd && "Something goes wrong!");

  MSMangleContextExtensions *msMangler = 
    CGM.getCXXABI().getMangleContext().getMsExtensions();
  assert(msMangler && "ExitMSCXXTryStmt must be called only for MS C++ ABI!");
  
  llvm::BasicBlock *oldBB = Builder.GetInsertBlock();
  
  llvm::Type *handlerTy = getHandlerType(CGM);

  llvm::Value *restoringState = 0;
  size_t index = -1;
  if (!EHState.LastEntries.empty()) {
    MSEHState::UnwindTableTy::iterator last = EHState.LastEntries.back();
    index = std::distance(EHState.GlobalUnwindTable.begin(), last); 
  }

  restoringState = llvm::ConstantInt::get(Int32Ty, index);
  llvm::Instruction *store = 
    EHState.AddCatchEntryInUnwindTable(index, restoringState);

  llvm::BasicBlock *brBlock = getMSInvokeDestImpl();
  // In MS do it in straight way.
  for (unsigned I = 0; I != NumHandlers; ++I) {
    llvm::SmallString<256>  catchHandlerName;
    llvm::raw_svector_ostream stream(catchHandlerName);

    msMangler->mangleEHCatchFunction(fd, EHState.EHManglingCounter, stream);
    EHState.EHManglingCounter++;

    stream.flush();
    StringRef mangledCatchName(catchHandlerName);

    const CXXCatchStmt *C = S.getHandler(I);
    llvm::BasicBlock *entryBB = 
      llvm::BasicBlock::Create(CGM.getLLVMContext(),
                               mangledCatchName,
                               CurFn);
    Builder.SetInsertPoint(entryBB);

    EmitStmt(C->getHandlerBlock());

    if (I) {
      Builder.Insert(store->clone());
    } else {
      Builder.Insert(store);
      doHandlerReachable(Builder, brBlock, entryBB);
    }

    insertCatchRet(*this);
    
    if (I) {
      doHandlerReachable(Builder, brBlock, entryBB);
    }

    QualType CaughtType = C->getCaughtType();
    GenerateCatchHandler(CaughtType, handlerTy,
                         llvm::BlockAddress::get(CurFn, entryBB));
  }

  Builder.CreateUnreachable();
  EHState.LastCatchHandler = Builder.GetInsertBlock();

  EHState.LastEntries.pop_back();

  Builder.SetInsertPoint(oldBB);

  llvm::BasicBlock *tryEnd = EHState.GenerateTryEndBlock(fd, msMangler);
  
  Builder.CreateBr(tryEnd);
  
  Builder.SetInsertPoint(tryEnd);

  EHState.TryLevel--;

  // Restore state after exiting from try block.
  EHState.CreateStateStore(restoringState);

  GenerateTryBlockTableEntry();

  if (!EHState.LastEntries.empty()) {
    EHState.LastEntries.pop_back();
  }
  EHState.LocalUnwindTable.pop_back();
}

// r4start
void CodeGenFunction::UpdateEHInfo(const Decl *TargetDecl, llvm::Value *This) {
  // Function call or we not in try stmt.
  if (!This) {
    return;
  }

  if (EHState.LocalUnwindTable.empty()) {
    EHState.LocalUnwindTable.push_back(MSEHState::UnwindEntryRefList());
  }
  
  Decl::Kind kind;
  const CXXMethodDecl *MD = dyn_cast_or_null<CXXMethodDecl>(TargetDecl);
      
  if (MD) {
    kind = MD->getKind();
    if (kind == Decl::CXXConstructor) {
      const CXXRecordDecl *parent = MD->getParent();
      llvm::Value *dtor = 
        CGM.GetAddrOfCXXDestructor(parent->getDestructor(), Dtor_Base);
      EHState.SetMSTryState();
      EHState.GlobalUnwindTable.back().ThisPtr = This;
      EHState.GlobalUnwindTable.back().ReleaseFunc = dtor;
    } else if (kind == Decl::CXXDestructor && 
               !EHState.LocalUnwindTable.back().empty()) {
      llvm::Value *restoringVal = 0;
      // TODO: get rid of call size().
      if (EHState.LocalUnwindTable.back().size() > 1) {
        restoringVal = 
          (*(++EHState.LocalUnwindTable.back().rbegin()))->Store->
                                                                 getOperand(0);
      } else {
        restoringVal = llvm::ConstantInt::get(Int32Ty, -1);
      }
      EHState.CreateStateStore(restoringVal);
      EHState.LocalUnwindTable.back().pop_back();
    }
  }
}

llvm::BasicBlock *CodeGenFunction::getMSInvokeDestImpl() {
  if (EHState.CachedLPad) {
    return EHState.CachedLPad;
  }

  if (!EHState.LandingPads.empty()) {
    EHState.CachedLPad = EHState.LandingPads.back();
    return EHState.CachedLPad;
  }

  // Create and configure the landing pad.
  EHState.CachedLPad = createBasicBlock("lpad");
  EHState.LandingPads.push_back(EHState.CachedLPad);

  // Save the current IR generation state.
  CGBuilderTy::InsertPoint savedIP = Builder.saveAndClearIP();

  EmitBlock(EHState.CachedLPad);

  llvm::LandingPadInst *lPad = 
      Builder.CreateLandingPad(
                llvm::StructType::get(Int8PtrTy, Int32Ty, NULL),
                llvm::ConstantExpr::getBitCast(getFakePersonality(*this),
                                               CGM.Int8PtrTy),
                0);
  lPad->setCleanup(true);
  // Restore the old IR generation state.
  Builder.restoreIP(savedIP);

  return EHState.CachedLPad;
}
