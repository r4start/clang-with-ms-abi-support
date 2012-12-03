// RUN: %clang_cc1  -emit-llvm %s -o - -cxx-abi microsoft -triple=i686-pc-win32 | FileCheck %s
// r4start

// CHECK: define i32 @wmain(i32, i16**, i16**)
int wmain(int, wchar_t **, wchar_t **) {
  return 0;
}