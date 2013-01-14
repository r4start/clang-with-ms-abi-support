//===-- VBTableBuilder.h - C++ vbtable layout builder for MS ABI --*- C++ -*-=//
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

#ifndef LLVM_CLANG_AST_VBTABLEBUILDER_H
#define LLVM_CLANG_AST_VBTABLEBUILDER_H

#include "clang/AST/CXXInheritance.h"
#include "clang/AST/GlobalDecl.h"
#include "clang/AST/RecordLayout.h"
#include "llvm/ADT/DenseMap.h"

/// r4start
namespace clang {

class VBTableContext {
public:
  struct VBTableEntry : 
   public std::binary_function<VBTableEntry, VBTableEntry, bool> {
    uint32_t index;
    int32_t offset;

    VBTableEntry() : index(-1), offset(-1) {}

    VBTableEntry(uint32_t Index, int32_t Offest) :
      index(Index), offset(Offest) {}

    bool operator () (const VBTableEntry &LHS, const VBTableEntry &RHS) {
      return (LHS.index < RHS.index);
    }
  };
  
  typedef std::map<VBTableEntry, const CXXRecordDecl *, VBTableEntry> BaseVBTableTy;
  struct VBTableHolder {
    BaseVBTableTy VBTable;
    CharUnits VBPtrOffset;
    CharUnits HolderOffset;
    bool VirtualHolder;
    const CXXRecordDecl *EnumeratorRD;
    const CXXRecordDecl *HolderRD;
    const CXXRecordDecl *OwnerRD;
  };
  typedef llvm::SmallVector<VBTableHolder, 3> VBTableTy;

private:

  /// We have such table:
  /// (1)Vbtable holder
  /// (2)Base class
  /// (3)Index of base in vbtable
  /// (4)Base class offset from vbtable position.
  llvm::DenseMap<const CXXRecordDecl *, VBTableTy > VBTables;

  VBTableHolder ComputeVBTable(VBTableTy& VBTableMap,
                      const ASTRecordLayout& MDLayout,
                      const CXXRecordDecl *EnumeratorRD,
                      const CXXRecordDecl *OwnerRD,
                      const CXXRecordDecl *Holder,
                      bool VirtualHolder,
                      CharUnits HolderOffset);

  void RecomputeBaseVBTables(VBTableTy& VBTableMap, 
                             const ASTRecordLayout& MDLayout,
                             const CXXRecordDecl *Base,
                             bool VirtualHolder);
  void ComputeVBTables(const CXXRecordDecl *RD);

public:

  const VBTableTy &getVBTables(const CXXRecordDecl *RD) {
    ComputeVBTables(RD);
    return VBTables[RD];
  }

  const VBTableEntry &getEntryFromPrimaryVBTable(const CXXRecordDecl *RD, 
                                                 const CXXRecordDecl *VBase);
  CharUnits getPrimaryVBPtrOffset(const CXXRecordDecl* RD);
  CharUnits getPrimaryHolderOffset(const CXXRecordDecl* RD);
};

}

#endif 