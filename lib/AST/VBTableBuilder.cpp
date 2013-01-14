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

VBTableContext::VBTableHolder 
     VBTableContext::ComputeVBTable(VBTableTy& VBTableMap,
                                    const ASTRecordLayout& MostDerivedLayout,
                                    const CXXRecordDecl *EnumeratorRD,
                                    const CXXRecordDecl *OwnerRD,
                                    const CXXRecordDecl *HolderRD,
                                    bool VirtualHolder,
                                    CharUnits HolderOffset) {
  VBTableHolder VBTHolder;
  ASTContext& Context = HolderRD->getASTContext();
  const ASTRecordLayout& HolderLayout = Context.getASTRecordLayout(HolderRD);
  CharUnits VBPtrOffset = HolderLayout.getVBPtrOffset();
  CharUnits VBPtrOffsetInRD = VBPtrOffset + HolderOffset;
  VBTHolder.VBTable.insert(std::make_pair(
    VBTableContext::VBTableEntry(0, -VBPtrOffset.getQuantity()), HolderRD));

  int idx = 1;
  for (CXXRecordDecl::base_class_const_iterator I = EnumeratorRD->vbases_begin(),
       E = EnumeratorRD->vbases_end(); I != E; ++I, ++idx) {
    const CXXRecordDecl *VBase = I->getType()->getAsCXXRecordDecl();

    CharUnits VBaseOffset = MostDerivedLayout.getVBaseClassOffset(VBase) - VBPtrOffsetInRD;

    VBTHolder.VBTable.insert(std::make_pair(
      VBTableContext::VBTableEntry(idx, VBaseOffset.getQuantity()),
                      VBase));
  }

  VBTHolder.EnumeratorRD = EnumeratorRD;
  VBTHolder.HolderRD = HolderRD;
  VBTHolder.OwnerRD = OwnerRD;
  VBTHolder.VirtualHolder = VirtualHolder;
  VBTHolder.VBPtrOffset = VBPtrOffsetInRD;
  VBTHolder.HolderOffset = HolderOffset;
  return VBTHolder;
}

void VBTableContext::RecomputeBaseVBTables(VBTableTy& VBTableMap, 
                                   const ASTRecordLayout& MDLayout,
                                   const CXXRecordDecl *Base,
                                   bool VirtualHolder) 
{
  if (Base->getNumVBases() == 0)
    return;
  CharUnits BaseOffset;
  if (VirtualHolder)
    BaseOffset = MDLayout.getVBaseClassOffset(Base);
  else
    BaseOffset = MDLayout.getBaseClassOffset(Base);
  ComputeVBTables(Base);
  for (VBTableTy::const_iterator I = VBTables[Base].begin(), 
        E = VBTables[Base].end(); I != E; ++I) {
    if (I->VirtualHolder)
      continue;
    CharUnits VBTableOffset = BaseOffset + I->HolderOffset;
    VBTableMap.push_back(ComputeVBTable(VBTableMap, MDLayout, 
                         I->EnumeratorRD, I->OwnerRD, 
                         I->HolderRD, VirtualHolder, VBTableOffset));
  }
}

static bool VBTableOrder(const VBTableContext::VBTableHolder& lhs, 
                         const VBTableContext::VBTableHolder& rhs) {
  return lhs.VBPtrOffset < rhs.VBPtrOffset;
}

void VBTableContext::ComputeVBTables(const CXXRecordDecl *RD) {
  assert(RD->getNumVBases() && "Class must have virtual bases");
  if (VBTables.count(RD))
    return;

  VBTableTy &VBTableMap = VBTables[RD];
  const ASTRecordLayout& RDLayout = RD->getASTContext().getASTRecordLayout(RD);
  bool HaveVBTable = RDLayout.getVBPtrOffset() != CharUnits::fromQuantity(-1);

  if (!HaveVBTable) {
    // if class have it's own vbtable it can't be non-virtually derivered from
    // any class with vbtable, because it's vbtable will be shared in this case
    for (CXXRecordDecl::base_class_const_iterator I = RD->bases_begin(),
         E = RD->bases_end(); I != E; ++I) {
      // We handle all vbase vbtables separatly because vbase offset in base class
      // can be different from same vbase offset in derived class
      // In fact, vbases are transparently projected in derived layout
      if (I->isVirtual())
        continue;
      const CXXRecordDecl *Base = I->getType()->getAsCXXRecordDecl();
      RecomputeBaseVBTables(VBTableMap, RDLayout, Base, false);
    }
  }
  for (CXXRecordDecl::base_class_const_iterator I = RD->vbases_begin(),
       E = RD->vbases_end(); I != E; ++I) {
    const CXXRecordDecl *Base = I->getType()->getAsCXXRecordDecl();
    RecomputeBaseVBTables(VBTableMap, RDLayout, Base, true);
  }

  if (HaveVBTable) {
    VBTableMap.insert(VBTableMap.begin(), ComputeVBTable(VBTableMap, RDLayout, 
                                        RD, RD, RD,
                                        false, CharUnits::Zero()));
  } else {
    assert(!VBTableMap.empty() && "Class don't have vbtable");
    std::sort(VBTableMap.begin(), VBTableMap.end(), VBTableOrder);
    VBTableHolder& PrimaryVBT = *VBTableMap.begin();
    const CXXRecordDecl *OwnerRD = RD;
    if (VBTableMap.size() != 1)
      OwnerRD = PrimaryVBT.OwnerRD;
    PrimaryVBT = ComputeVBTable(VBTableMap, RDLayout, 
                                RD, OwnerRD, PrimaryVBT.HolderRD,
                                false, PrimaryVBT.HolderOffset);
  }
}

const VBTableContext::VBTableEntry &
VBTableContext::getEntryFromPrimaryVBTable(const CXXRecordDecl *RD,
                                    const CXXRecordDecl *Base) {
  assert(RD->getNumVBases() && "Class must have virtual bases!");
  ComputeVBTables(RD);
  const BaseVBTableTy &VBTable = VBTables[RD].begin()->VBTable;

  for (BaseVBTableTy::const_iterator I = VBTable.begin(),
       E = VBTable.end(); I != E; ++I) {
    if (I->second == Base)
      return I->first;
  }

  llvm_unreachable("Can not find entry for base in vbtable!");
}

CharUnits VBTableContext::getPrimaryVBPtrOffset(const CXXRecordDecl* RD) {
  ComputeVBTables(RD);
  return VBTables[RD].begin()->VBPtrOffset;
  }

CharUnits VBTableContext::getPrimaryHolderOffset(const CXXRecordDecl* RD) {
  ComputeVBTables(RD);
  return VBTables[RD].begin()->HolderOffset;
}
