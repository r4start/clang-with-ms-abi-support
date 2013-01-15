//===------ VBTableBuilder.cpp - C++ vbtable layout builder for MS ABI ------=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This contains code dealing with generation of the layout of 
// virtual bases table.
//
//===----------------------------------------------------------------------===//

/// r4start
#include "clang/AST/ASTContext.h"
#include "clang/AST/VBTableBuilder.h"

#include <vector>

using namespace clang;

namespace {

typedef llvm::SmallSet<const CXXRecordDecl *, 8> ClassesContainerTy;

class VBTableBuilder {
  VBTableContext::BaseVBTableTy VBTable;

  const CXXRecordDecl *MostDerived;
  const CXXRecordDecl *LayoutClass;

  bool IsTablePrimary;

  ASTContext &Ctx;

  void Layout();
  void LayoutSingleVBTable();
  void LayoutForMultiplyVBTables();

public:
  VBTableBuilder(ASTContext &C, const CXXRecordDecl *MostDerived,
                 const CXXRecordDecl *LayoutClass, 
                 bool IsPrimary = false);

  const VBTableContext::BaseVBTableTy &getVBTable() const {
    return VBTable;
  }
};

}

VBTableBuilder::VBTableBuilder(ASTContext &C, const CXXRecordDecl *MostDerived,
                               const CXXRecordDecl *LayoutClass, 
                               bool IsPrimary)
  : Ctx(C), MostDerived(MostDerived), LayoutClass(LayoutClass), 
    IsTablePrimary(IsPrimary) {
  Layout();
}

static CharUnits GetClassOffset(const ASTContext &Ctx,
                                const CXXRecordDecl *RD,
                                const CXXRecordDecl *Base) {
  assert(RD != Base && 
         "GetClassOffset can`t work if RD equal Base");

  CharUnits BaseOffset;

  if (RD->isVirtuallyDerivedFrom(const_cast<CXXRecordDecl *>(Base))) {
    const ASTRecordLayout &Layout = Ctx.getASTRecordLayout(RD);
    return Layout.getVBaseClassOffset(Base);
  }

  CXXBasePaths InhPaths;
  InhPaths.setOrigin(const_cast<CXXRecordDecl *>(RD));
  RD->lookupInBases(&CXXRecordDecl::FindBaseClass, 
                 const_cast<CXXRecordDecl *>(Base), InhPaths);

  assert(InhPaths.begin() != InhPaths.end() &&
         "Can`t find inheritance path from RD to Base!");
  
  CXXBasePath& Path = InhPaths.front();

  for (CXXBasePath::const_iterator I = Path.begin(),
       E = Path.end(); I != E; ++I) {
    const CXXRecordDecl *BaseElem = I->Base->getType()->getAsCXXRecordDecl();
    const ASTRecordLayout &Layout = Ctx.getASTRecordLayout(I->Class);

    if (I->Base->isVirtual())
      BaseOffset += Layout.getVBaseClassOffset(BaseElem);
    else
      BaseOffset += Layout.getBaseClassOffset(BaseElem);
  }

  return BaseOffset;
}

static uint32_t GetVBTableOffset(ASTContext &C, const CXXRecordDecl *RD,
                                int64_t StartOffset) {
  const ASTRecordLayout &Layout = C.getASTRecordLayout(RD);

  if (!RD->getNumVBases())
    return -1;
  if (Layout.getVBPtrOffset().getQuantity() != -1)
    return Layout.getVBPtrOffset().getQuantity() + StartOffset;

  uint32_t Offset = -1;
  for (CXXRecordDecl::base_class_const_iterator I = RD->bases_begin(),
    E = RD->bases_end(); I != E; ++I) {
      CXXRecordDecl *Base = I->getType()->getAsCXXRecordDecl();

      if (I->isVirtual())
        StartOffset += Layout.getVBaseClassOffset(Base).getQuantity();
      else
        StartOffset += Layout.getBaseClassOffset(Base).getQuantity();

      Offset = GetVBTableOffset(C, Base, StartOffset);
      if (Offset != -1)
        return Offset;
  }

  return Offset;
}

void VBTableBuilder::LayoutSingleVBTable() {
  uint32_t offset = GetVBTableOffset(Ctx, MostDerived, 0);

  uint32_t offsetInHolder;
  if (MostDerived == LayoutClass) {
    offsetInHolder = offset;
  } else {
    offsetInHolder = GetVBTableOffset(Ctx, LayoutClass, 0);
  }

  VBTable.insert(std::make_pair(
                              VBTableContext::VBTableEntry(0, -offsetInHolder),
                              MostDerived));

  const ASTRecordLayout &Layout = Ctx.getASTRecordLayout(MostDerived);

  int idx = 1;
  for (CXXRecordDecl::base_class_const_iterator I = MostDerived->vbases_begin(),
       E = MostDerived->vbases_end(); I != E; ++I, ++idx) {
    const CXXRecordDecl *VBase = I->getType()->getAsCXXRecordDecl();

    int32_t BaseOffset = Layout.getVBaseClassOffset(VBase).getQuantity();

    VBTable.insert(std::make_pair(
                      VBTableContext::VBTableEntry(idx, BaseOffset - offset),
                      VBase));
  }
}

void VBTableBuilder::LayoutForMultiplyVBTables() {
  uint32_t Offset = GetVBTableOffset(Ctx, LayoutClass, 0);
  uint32_t ClassOffset = GetClassOffset(MostDerived->getASTContext(),
                                        MostDerived, LayoutClass).getQuantity();

  VBTable.insert(std::make_pair(VBTableContext::VBTableEntry(0, -Offset),
                                MostDerived));

  Offset += ClassOffset;
  const ASTRecordLayout &Layout = Ctx.getASTRecordLayout(MostDerived);
  int idx = 1;
  
  for (CXXRecordDecl::base_class_const_iterator I = LayoutClass->vbases_begin(),
       E = LayoutClass->vbases_end(); I != E; ++I, ++idx) {
    const CXXRecordDecl *VBase = I->getType()->getAsCXXRecordDecl();

    int32_t BaseOffset = Layout.getVBaseClassOffset(VBase).getQuantity();

    VBTable.insert(std::make_pair(
                        VBTableContext::VBTableEntry(idx, BaseOffset - Offset),
                        VBase));
  }
}

void VBTableBuilder::Layout() {
  if (MostDerived == LayoutClass || IsTablePrimary)
    LayoutSingleVBTable();
  else
    LayoutForMultiplyVBTables();
}

void VBTableContext::ComputeVBTable(const CXXRecordDecl *RD,
                                    const CXXRecordDecl *Base,
                                    bool IsPrimaryTable) {
  if (VBTables.count(RD))
    if (VBTables[RD].count(Base))
      return;
  VBTableBuilder Builder(RD->getASTContext(), RD, Base, IsPrimaryTable);
  
  VBTables.insert(std::make_pair(RD, VBTableTy()));
  VBTables[RD].insert(std::make_pair(Base, Builder.getVBTable()));
}

static void TraverseBases(ASTContext &Ctx, const CXXRecordDecl *Class,
                          ClassesContainerTy &DefferredClasses) {
  for (CXXRecordDecl::base_class_const_iterator I = Class->bases_begin(),
       E = Class->bases_end(); I != E; ++I) {
    const CXXRecordDecl *BaseDecl = I->getType()->getAsCXXRecordDecl();
    const ASTRecordLayout &Layout = Ctx.getASTRecordLayout(BaseDecl);
    if (Layout.getVBPtrOffset() != CharUnits::fromQuantity(-1))
      DefferredClasses.insert(BaseDecl);
    TraverseBases(Ctx, BaseDecl, DefferredClasses);
  }

  const ASTRecordLayout &Layout = Ctx.getASTRecordLayout(Class);
  if (Layout.getVBPtrOffset() != CharUnits::fromQuantity(-1))
    DefferredClasses.insert(Class);
}

static const CXXRecordDecl *
getSharedVBTableHolder(ASTContext &Ctx, const CXXRecordDecl *Class,
                       ClassesContainerTy &DefferredClasses) {
  const ASTRecordLayout &Layout = Ctx.getASTRecordLayout(Class);
  if (Layout.getVBPtrOffset().getQuantity() != -1) {
    return 0;
  }

  const CXXRecordDecl *sharedHolder = 0;
  CharUnits offset;
  CharUnits holderOffset;

  for (ClassesContainerTy::iterator i = DefferredClasses.begin(), 
       e = DefferredClasses.end(); i != e; ++i) {
    if (Class->isVirtuallyDerivedFrom(*i)) {
      continue;
    }

    offset = GetClassOffset(Ctx, Class, *i);
    if (!sharedHolder || offset < holderOffset) {
      sharedHolder = *i;
      holderOffset = offset;
    }
  }

  return sharedHolder;
}

void VBTableContext::ComputeVBTables(const CXXRecordDecl *RD) {
  assert(RD->getNumVBases() && "Class must have virtual bases");
  if (VBTables.count(RD))
    return;

  ClassesContainerTy ClassWithVBTables;
  TraverseBases(RD->getASTContext(), RD, ClassWithVBTables);

  const CXXRecordDecl *sharedVBTableHolder = 
    getSharedVBTableHolder(RD->getASTContext(), RD, ClassWithVBTables);

  for (ClassesContainerTy::iterator I = ClassWithVBTables.begin(), 
       E = ClassWithVBTables.end(); I != E; ++I) {
    const CXXRecordDecl *Class = *I;

    if (Class == RD)
      ComputeVBTable(Class, Class);
    else if (Class == sharedVBTableHolder)
      ComputeVBTable(RD, Class, true);
    else
      ComputeVBTable(RD, Class);
  }
}

const VBTableContext::VBTableEntry &
VBTableContext::getEntryFromVBTable(const CXXRecordDecl *RD,
                                    const CXXRecordDecl *LayoutClass,
                                    const CXXRecordDecl *Base) {
  assert(RD->getNumVBases() && "Class must have virtual bases!");
  ComputeVBTables(RD);
  const BaseVBTableTy &Table = VBTables[RD][LayoutClass];

  for (BaseVBTableTy::const_iterator I = Table.begin(),
       E = Table.end(); I != E; ++I) {
    if (I->second == Base)
      return I->first;
  }

  llvm_unreachable("Can not find entry for base in vbtable!");
}