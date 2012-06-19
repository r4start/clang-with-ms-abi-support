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
  typedef llvm::DenseMap<const CXXRecordDecl *, BaseVBTableTy > VBTableTy;

  /// We have such table:
  /// (1)Vbtable holder
  /// (2)Base class
  /// (3)Index of base in vbtable
  /// (4)Base class offset from vbtable position.
  llvm::DenseMap<const CXXRecordDecl *, VBTableTy > VBTables;

private:

  void ComputeVBTable(const CXXRecordDecl *RD, const CXXRecordDecl *Base);
  void ComputeVBTables(const CXXRecordDecl *RD);

public:

  const BaseVBTableTy &getVBTable(const CXXRecordDecl *RD,
                                  const CXXRecordDecl *Base) {
    ComputeVBTable(RD, Base);
    return VBTables[RD][Base];
  }

  const VBTableTy &getVBTables(const CXXRecordDecl *RD) {
    ComputeVBTables(RD);
    return VBTables[RD];
  }

  const VBTableEntry &getEntryFromVBTable(const CXXRecordDecl *RD,
                                          const CXXRecordDecl *LayoutClass,
                                          const CXXRecordDecl *Base);
};

}

#endif 