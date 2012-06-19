//===--- MicrosoftCXXABI.cpp - Emit LLVM Code from ASTs for a Module ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This provides C++ code generation targeting the Microsoft Visual C++ ABI.
// The class in this file generates structures that follow the Microsoft
// Visual C++ ABI, which is actually not very well documented at all outside
// of Microsoft.
//
//===----------------------------------------------------------------------===//

#include "CGCXXABI.h"
#include "CodeGenModule.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"

using namespace clang;
using namespace CodeGen;

// r4start
namespace {

class MicrosoftCXXABI : public CGCXXABI {
public:
  MicrosoftCXXABI(CodeGenModule &CGM) : CGCXXABI(CGM) {}

  StringRef GetPureVirtualCallName() { return "_purecall"; }

  void BuildConstructorSignature(const CXXConstructorDecl *Ctor,
                                 CXXCtorType Type,
                                 CanQualType &ResTy,
                                 SmallVectorImpl<CanQualType> &ArgTys);

  void BuildDestructorSignature(const CXXDestructorDecl *Ctor,
                                CXXDtorType Type,
                                CanQualType &ResTy,
                                SmallVectorImpl<CanQualType> &ArgTys);

  void BuildInstanceFunctionParams(CodeGenFunction &CGF,
                                   QualType &ResTy,
                                   FunctionArgList &Params);

  void EmitInstanceFunctionProlog(CodeGenFunction &CGF);

  void EmitGuardedInit(CodeGenFunction &CGF, const VarDecl &D,
                       llvm::GlobalVariable *DeclPtr,
                       bool PerformInit);

  void EmitVTables(const CXXRecordDecl *Class);


  // ==== Notes on array cookies =========
  //
  // MSVC seems to only use cookies when the class has a destructor; a
  // two-argument usual array deallocation function isn't sufficient.
  //
  // For example, this code prints "100" and "1":
  //   struct A {
  //     char x;
  //     void *operator new[](size_t sz) {
  //       printf("%u\n", sz);
  //       return malloc(sz);
  //     }
  //     void operator delete[](void *p, size_t sz) {
  //       printf("%u\n", sz);
  //       free(p);
  //     }
  //   };
  //   int main() {
  //     A *p = new A[100];
  //     delete[] p;
  //   }
  // Whereas it prints "104" and "104" if you give A a destructor.

  bool requiresArrayCookie(const CXXDeleteExpr *expr, QualType elementType);
  bool requiresArrayCookie(const CXXNewExpr *expr);
  CharUnits getArrayCookieSizeImpl(QualType type);
  llvm::Value *InitializeArrayCookie(CodeGenFunction &CGF,
                                     llvm::Value *NewPtr,
                                     llvm::Value *NumElements,
                                     const CXXNewExpr *expr,
                                     QualType ElementType);
  llvm::Value *readArrayCookieImpl(CodeGenFunction &CGF,
                                   llvm::Value *allocPtr,
                                   CharUnits cookieSize);

  static bool NeedThisReturn(GlobalDecl GD);
};

}

bool MicrosoftCXXABI::NeedThisReturn(GlobalDecl GD) {
  const CXXMethodDecl* MD = cast<CXXMethodDecl>(GD.getDecl());
  return isa<CXXConstructorDecl>(MD);
}

void MicrosoftCXXABI::BuildConstructorSignature(const CXXConstructorDecl *Ctor,
                                                CXXCtorType Type,
                                                CanQualType &ResTy,
                                   llvm::SmallVectorImpl<CanQualType> &ArgTys) {
    // 'this' is already in place
    // Ctor returns this ptr
    ResTy = ArgTys[0];

    const CXXRecordDecl *Class = Ctor->getParent();
    if (!Class->getNumVBases())
      return;
    ArgTys.push_back(CGM.getContext().Char32Ty);
}

void MicrosoftCXXABI::BuildDestructorSignature(const CXXDestructorDecl *Ctor,
                                               CXXDtorType Type,
                                               CanQualType &ResTy,
                                   llvm::SmallVectorImpl<CanQualType> &ArgTys) {
  // 'this' is already in place
  // TODO: 'for base' flag
  // TODO: add RTTI if need
}

void MicrosoftCXXABI::BuildInstanceFunctionParams(CodeGenFunction &CGF,
                                                  QualType &ResTy,
                                                  FunctionArgList &Params) {
    BuildThisParam(CGF, Params);
    if (NeedThisReturn(CGF.CurGD)) {
      ResTy = Params[0]->getType();
    }

    const CXXMethodDecl *MD = cast<CXXMethodDecl>(CGF.CurGD.getDecl());
    if (!MD->getParent()->getNumVBases() || !isa<CXXConstructorDecl>(MD))
      return;
    ImplicitParamDecl *FlagDecl
      = ImplicitParamDecl::Create(CGM.getContext(), 0, MD->getLocation(),
      &CGM.getContext().Idents.get("ctor.flag"),
      CGM.getContext().Char32Ty);
    Params.push_back(FlagDecl);
}

void MicrosoftCXXABI::EmitInstanceFunctionProlog(CodeGenFunction &CGF) {
  EmitThisParam(CGF);

  if (NeedThisReturn(CGF.CurGD)) {
    CGF.Builder.CreateStore(getThisValue(CGF), CGF.ReturnValue);
  }
  // TODO: 'for base' flag
}


bool MicrosoftCXXABI::requiresArrayCookie(const CXXDeleteExpr *expr,
                                   QualType elementType) {
  // Microsoft seems to completely ignore the possibility of a
  // two-argument usual deallocation function.
  return elementType.isDestructedType();
}

bool MicrosoftCXXABI::requiresArrayCookie(const CXXNewExpr *expr) {
  // Microsoft seems to completely ignore the possibility of a
  // two-argument usual deallocation function.
  return expr->getAllocatedType().isDestructedType();
}

CharUnits MicrosoftCXXABI::getArrayCookieSizeImpl(QualType type) {
  // The array cookie is always a size_t; we then pad that out to the
  // alignment of the element type.
  ASTContext &Ctx = getContext();
  return std::max(Ctx.getTypeSizeInChars(Ctx.getSizeType()),
                  Ctx.getTypeAlignInChars(type));
}

llvm::Value *MicrosoftCXXABI::readArrayCookieImpl(CodeGenFunction &CGF,
                                                  llvm::Value *allocPtr,
                                                  CharUnits cookieSize) {
  unsigned AS = cast<llvm::PointerType>(allocPtr->getType())->getAddressSpace();
  llvm::Value *numElementsPtr =
    CGF.Builder.CreateBitCast(allocPtr, CGF.SizeTy->getPointerTo(AS));
  return CGF.Builder.CreateLoad(numElementsPtr);
}

llvm::Value* MicrosoftCXXABI::InitializeArrayCookie(CodeGenFunction &CGF,
                                                    llvm::Value *newPtr,
                                                    llvm::Value *numElements,
                                                    const CXXNewExpr *expr,
                                                    QualType elementType) {
  assert(requiresArrayCookie(expr));

  // The size of the cookie.
  CharUnits cookieSize = getArrayCookieSizeImpl(elementType);

  // Compute an offset to the cookie.
  llvm::Value *cookiePtr = newPtr;

  // Write the number of elements into the appropriate slot.
  unsigned AS = cast<llvm::PointerType>(newPtr->getType())->getAddressSpace();
  llvm::Value *numElementsPtr
    = CGF.Builder.CreateBitCast(cookiePtr, CGF.SizeTy->getPointerTo(AS));
  CGF.Builder.CreateStore(numElements, numElementsPtr);

  // Finally, compute a pointer to the actual data buffer by skipping
  // over the cookie completely.
  return CGF.Builder.CreateConstInBoundsGEP1_64(newPtr,
                                                cookieSize.getQuantity());
}

void MicrosoftCXXABI::EmitGuardedInit(CodeGenFunction &CGF, const VarDecl &D,
                                      llvm::GlobalVariable *DeclPtr,
                                      bool PerformInit) {
  // FIXME: this code was only tested for global initialization.
  // Not sure whether we want thread-safe static local variables as VS
  // doesn't make them thread-safe.

  // Emit the initializer and add a global destructor if appropriate.
  CGF.EmitCXXGlobalVarDeclInit(D, DeclPtr, PerformInit);
}

void MicrosoftCXXABI::EmitVTables(const CXXRecordDecl *Class) {
  // FIXME: implement
}

CGCXXABI *clang::CodeGen::CreateMicrosoftCXXABI(CodeGenModule &CGM) {
  return new MicrosoftCXXABI(CGM);
}

