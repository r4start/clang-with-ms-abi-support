// RUN: %clang_cc1 -fms-extensions -fblocks -emit-llvm %s -o - -cxx-abi microsoft -triple=i386-pc-win32 | FileCheck %s

namespace test1 {

struct f {
  virtual int asd() {return -90;}
};

struct s : public virtual f{
  s(){}
  virtual int asd() {return -9;}
};

void test() { s ss; }

}

// CHECK: %"struct.test1::s" = type { i32*, [4 x i8], %"struct.test1::f" }
// CHECK: %"struct.test1::f" = type { i32 (...)** }

// CHECK: %"struct.test2::s" = type { i32*, [4 x i8], %"struct.test2::f", [4 x i8], %"struct.test2::fd" }
// CHECK: %"struct.test2::f" = type { i32 (...)** }
// CHECK: %"struct.test2::fd" = type { i32 (...)** }

// FIXME: Fix vb-table name mangling with namespaces.
// CHECK: @"\01??_8s@test2@@7B@" = unnamed_addr constant [3 x i32] [i32 0, i32 8, i32 16]

// CHECK: @"\01??_8s@test1@@7B@" = unnamed_addr constant [2 x i32] [i32 0, i32 8]

// CHECK: define linkonce_odr %"struct.test1::s"* @"\01??0s@test1@@QAE@XZ"(%"struct.test1::s"* %this, i32 zeroext %ctor.flag)

// CHECK:        %vtordisp.this = bitcast %"struct.test1::s"* %this1 to i8*
// CHECK-NEXT:   %vbtable.ptr = getelementptr inbounds i8* %vtordisp.this, i32 0
// CHECK-NEXT:   %vbtable.i32.ptr = bitcast i8* %vbtable.ptr to i32*
// CHECK-NEXT:   %offset.from.table = getelementptr i32* %vbtable.i32.ptr, i32 1
// CHECK-NEXT:   %vb.offset = load i32* %offset.from.table
// CHECK-NEXT:   %vtordisp.val = sub i32 %vb.offset, 8
// CHECK-NEXT:   %5 = getelementptr i8* %vtordisp.this, i32 4
// CHECK-NEXT:   %6 = bitcast i8* %5 to i32*
// CHECK-NEXT:   store i32 %vtordisp.val, i32* %6

namespace test2 {

struct f {
  virtual int asd() {return -90;}
};

struct fd {
  virtual int asfd() {return -90;}
};

struct s : public virtual f, public virtual fd {
  s(){}
  virtual int asd() {return -9;}
  virtual int asfd() {return -9;}
};

void test2() { s ss; }

}

// CHECK: define linkonce_odr %"struct.test2::s"* @"\01??0s@test2@@QAE@XZ"(%"struct.test2::s"* %this, i32 zeroext %ctor.flag)

// CHECK:        %vtordisp.this = bitcast %"struct.test2::s"* %this1 to i8*
// CHECK-NEXT:   %vbtable.ptr = getelementptr inbounds i8* %vtordisp.this, i32 0
// CHECK-NEXT:   %vbtable.i32.ptr = bitcast i8* %vbtable.ptr to i32*
// CHECK-NEXT:   %offset.from.table = getelementptr i32* %vbtable.i32.ptr, i32 1
// CHECK-NEXT:   %vb.offset = load i32* %offset.from.table
// CHECK-NEXT:   %vtordisp.val = sub i32 %vb.offset, 8
// CHECK-NEXT:   %9 = getelementptr i8* %vtordisp.this, i32 4
// CHECK-NEXT:   %10 = bitcast i8* %9 to i32*
// CHECK-NEXT:   store i32 %vtordisp.val, i32* %10
// CHECK-NEXT:   %vtordisp.this5 = bitcast %"struct.test2::s"* %this1 to i8*
// CHECK-NEXT:   %vbtable.ptr6 = getelementptr inbounds i8* %vtordisp.this5, i32 0
// CHECK-NEXT:   %vbtable.i32.ptr7 = bitcast i8* %vbtable.ptr6 to i32*
// CHECK-NEXT:   %offset.from.table8 = getelementptr i32* %vbtable.i32.ptr7, i32 2
// CHECK-NEXT:   %vb.offset9 = load i32* %offset.from.table8
// CHECK-NEXT:   %vtordisp.val10 = sub i32 %vb.offset9, 16
// CHECK-NEXT:   %11 = getelementptr i8* %vtordisp.this5, i32 12
// CHECK-NEXT:   %12 = bitcast i8* %11 to i32*
// CHECK-NEXT:   store i32 %vtordisp.val10, i32* %12
