//===----- Attr.h - Enum values for C Attribute Kinds ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the attr::Kind enum
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_ATTRKINDS_H
#define LLVM_CLANG_ATTRKINDS_H

namespace clang {

namespace attr {

// Kind - This is a list of all the recognized kinds of attributes.
enum Kind {
#define ATTR(X) X,
#define LAST_INHERITABLE_ATTR(X) X, LAST_INHERITABLE = X,
#define LAST_INHERITABLE_PARAM_ATTR(X) X, LAST_INHERITABLE_PARAM = X,
#define LAST_MS_SA_ATTR(X) X, LAST_MS_SA = X,
#include "clang/Basic/AttrList.inc"
  NUM_ATTRS
};

} // end namespace attr
} // end namespace clang

#endif
