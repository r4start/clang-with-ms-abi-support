// RUN: %clang_cc1 -fms-extensions -fblocks -emit-llvm %s -o - -cxx-abi microsoft -triple=i386-pc-win32 | FileCheck %s

namespace test1 {

#pragma pack(push, 8)

struct A {
  virtual void foo(){}
};

#pragma pack(pop)

void test() { A a; }

}

namespace test2 {

#pragma pack(push, 8)

struct A {
  virtual void foo(){}
};

struct B : virtual A {
};

#pragma pack(pop)

void test() { B b; }

}

// FIXME: ??_7B@test2@@6B@
// FIXME: ??_R4B@test2@@6B@ 

// CHECK: @"\01??_7B@test2@@6B@" = linkonce_odr unnamed_addr constant [2 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4B@test2@@6B@" to i8*), i8* bitcast (void (%"struct.test2::A"*)* @"\01?foo@A@test2@@UAEXXZ" to i8*)]

// CHECK: @"\01??_7A@test1@@6B@" = linkonce_odr unnamed_addr constant [2 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4A@test1@@6B@" to i8*), i8* bitcast (void (%"struct.test1::A"*)* @"\01?foo@A@test1@@UAEXXZ" to i8*)]

// CHECK:      define linkonce_odr x86_thiscallcc %"struct.test1::A"* @"\01??0A@test1@@QAE@XZ"(%"struct.test1::A"* %this) unnamed_addr nounwind inlinehint align 2 {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %this.addr = alloca %"struct.test1::A"*, align 4
// CHECK-NEXT:   store %"struct.test1::A"* %this, %"struct.test1::A"** %this.addr, align 4
// CHECK-NEXT:   %this1 = load %"struct.test1::A"** %this.addr
// CHECK-NEXT:   %0 = bitcast %"struct.test1::A"* %this1 to i8*
// CHECK-NEXT:   %vfptr.field = getelementptr inbounds i8* %0, i64 0
// CHECK-NEXT:   %vfptr = bitcast i8* %vfptr.field to i8***
// CHECK-NEXT:   store i8** getelementptr inbounds ([2 x i8*]* @"\01??_7A@test1@@6B@", i64 0, i64 1), i8*** %vfptr
// CHECK-NEXT:   ret %"struct.test1::A"* %this1
// CHECK-NEXT: }

// CHECK: define linkonce_odr x86_thiscallcc %"struct.test2::B"* @"\01??0B@test2@@QAE@XZ"(%"struct.test2::B"* %this, i32 zeroext %ctor.flag) unnamed_addr nounwind inlinehint align 2 {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %retval = alloca %"struct.test2::B"*, align 4
// CHECK-NEXT:   %this.addr = alloca %"struct.test2::B"*, align 4
// CHECK-NEXT:   %ctor.flag.addr = alloca i32, align 4
// CHECK-NEXT:   store %"struct.test2::B"* %this, %"struct.test2::B"** %this.addr, align 4
// CHECK-NEXT:   store i32 %ctor.flag, i32* %ctor.flag.addr, align 4
// CHECK-NEXT:   %this1 = load %"struct.test2::B"** %this.addr
// CHECK-NEXT:   store %"struct.test2::B"* %this1, %"struct.test2::B"** %retval
// CHECK-NEXT:   %ctor.flag.cmp = icmp eq i32 0, %ctor.flag
// CHECK-NEXT:   br i1 %ctor.flag.cmp, label %vbtable.init, label %skip.vbtable.init
// CHECK: vbtable.init:                                     ; preds = %entry
// CHECK-NEXT:   %0 = bitcast %"struct.test2::B"* %this1 to i8*
// CHECK-NEXT:   %vbptr.field = getelementptr inbounds i8* %0, i64 0
// CHECK-NEXT:   %vbptr = bitcast i8* %vbptr.field to [2 x i32]**
// CHECK-NEXT:   store [2 x i32]* @"\01??_8B@test2@@7B@", [2 x i32]** %vbptr
// CHECK-NEXT:   %1 = bitcast %"struct.test2::B"* %this1 to i8*
// CHECK-NEXT:   %2 = getelementptr inbounds i8* %1, i64 4
// CHECK-NEXT:   %3 = bitcast i8* %2 to %"struct.test2::A"*
// CHECK-NEXT:   %call = call x86_thiscallcc %"struct.test2::A"* @"\01??0A@test2@@QAE@XZ"(%"struct.test2::A"* %3) nounwind
// CHECK-NEXT:   br label %skip.vbtable.init 
// CHECK: skip.vbtable.init:                                ; preds = %vbtable.init, %entry
// CHECK-NEXT:   %4 = bitcast %"struct.test2::B"* %this1 to i8*
// CHECK-NEXT:   %vfptr.field = getelementptr inbounds i8* %4, i64 4
// CHECK-NEXT:   %vfptr = bitcast i8* %vfptr.field to i8***
// CHECK-NEXT:   store i8** getelementptr inbounds ([2 x i8*]* @"\01??_7B@test2@@6B@", i64 0, i64 1), i8*** %vfptr
// CHECK-NEXT:   %5 = load %"struct.test2::B"** %retval
// CHECK-NEXT:   ret %"struct.test2::B"* %5
// CHECK-NEXT: }
