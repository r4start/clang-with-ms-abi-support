// RUN: %clang_cc1  -emit-llvm %s -o - -cxx-abi microsoft -fcxx-exceptions -fexceptions -triple=i686-pc-win32 | FileCheck %s
// r4start

struct A {
 A() { }
 ~A() {
 }
};

struct B {
 B() { }
 ~B() { }
};

struct C {
 C() { }
 ~C() { }
};

extern void s();

int test1() {
  try {
    A a, b;
  } catch (A &) {}
}

// CHECK: @"\01__unwindtable$test1@@YAHXZ" = internal constant [4 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test1@@YAHXZ", %"\01__unwindfunclet$test1@@YAHXZ$2") }, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test1@@YAHXZ", %"\01__unwindfunclet$test1@@YAHXZ$3") }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: define i32 @"\01?test1@@YAHXZ"() {
// CHECK-NEXT: entry:
// CHECK:   %try.id = alloca i32
// CHECK:   store i32 -1, i32* %try.id
// CHECK:   store i32 0, i32* %try.id
// CHECK-NEXT:   %call = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %a)

// CHECK:   store i32 1, i32* %try.id
// CHECK-NEXT:   %call2 = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %b)

// CHECK:   store i32 2, i32* %try.id
// CHECK-NEXT:   store i32 1, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   store i32 0, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__tryend$test1@@YAHXZ$1"

// CHECK: "\01__catch$test1@@YAHXZ$0":                      ; No predecessors!
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test1@@YAHXZ$1":                     ; preds = %call.cont1
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   %0 = load i32* %retval
// CHECK-NEXT:   ret i32 %0

// CHECK: "\01__unwindfunclet$test1@@YAHXZ$2":              ; No predecessors!
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test1@@YAHXZ$3"

// CHECK: "\01__unwindfunclet$test1@@YAHXZ$3":              ; preds = %"\01__unwindfunclet$test1@@YAHXZ$2"
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   call void @"\01__ehhandler$test1@@YAHXZ"() noreturn
// CHECK-NEXT:   unreachable
