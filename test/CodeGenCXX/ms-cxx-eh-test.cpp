// RUN: %clang_cc1  -emit-llvm %s -o - -cxx-abi microsoft -fcxx-exceptions -fexceptions -triple=i686-pc-win32 | FileCheck %s
// r4start

// CHECK: @"\01__unwindtable$test1@@YAHXZ" = internal constant [4 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test1@@YAHXZ", %"\01__unwindfunclet$test1@@YAHXZ$2") }, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test1@@YAHXZ", %"\01__unwindfunclet$test1@@YAHXZ$3") }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__unwindtable$test2@@YAHXZ" = internal constant [8 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__unwindfunclet$test2@@YAHXZ$4") }, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__unwindfunclet$test2@@YAHXZ$5") }, %unwind.map.entry { i32 2, i8* null }, %unwind.map.entry { i32 3, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__unwindfunclet$test2@@YAHXZ$6") }, %unwind.map.entry { i32 4, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__unwindfunclet$test2@@YAHXZ$7") }, %unwind.map.entry { i32 2, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__unwindtable$test3@@YAHXZ" = internal constant [8 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test3@@YAHXZ", %"\01__unwindfunclet$test3@@YAHXZ$4") }, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test3@@YAHXZ", %"\01__unwindfunclet$test3@@YAHXZ$5") }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 4, i8* blockaddress(@"\01?test3@@YAHXZ", %"\01__unwindfunclet$test3@@YAHXZ$6") }, %unwind.map.entry { i32 5, i8* blockaddress(@"\01?test3@@YAHXZ", %"\01__unwindfunclet$test3@@YAHXZ$7") }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__unwindtable$test4@@YAHXZ" = internal constant [12 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__unwindfunclet$test4@@YAHXZ$6") }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 2, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__unwindfunclet$test4@@YAHXZ$7") }, %unwind.map.entry { i32 3, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__unwindfunclet$test4@@YAHXZ$8") }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__unwindfunclet$test4@@YAHXZ$9") }, %unwind.map.entry { i32 6, i8* null }, %unwind.map.entry { i32 7, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__unwindfunclet$test4@@YAHXZ$10") }, %unwind.map.entry { i32 8, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__unwindfunclet$test4@@YAHXZ$11") }, %unwind.map.entry { i32 6, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__unwindtable$test5@@YAHXZ" = internal constant [13 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$6") }, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$7") }, %unwind.map.entry { i32 2, i8* null }, %unwind.map.entry { i32 3, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$8") }, %unwind.map.entry { i32 4, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$9") }, %unwind.map.entry { i32 2, i8* null }, %unwind.map.entry { i32 2, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$10") }, %unwind.map.entry { i32 7, i8* null }, %unwind.map.entry { i32 8, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$11") }, %unwind.map.entry { i32 9, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$12") }, %unwind.map.entry { i32 7, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

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
  return 0;
}

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
// CHECK-NEXT:   ret i32 0

// CHECK: "\01__unwindfunclet$test1@@YAHXZ$2":              ; No predecessors!
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test1@@YAHXZ$3"

// CHECK: "\01__unwindfunclet$test1@@YAHXZ$3":              ; preds = %"\01__unwindfunclet$test1@@YAHXZ$2"
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   call void @"\01__ehhandler$test1@@YAHXZ"() noreturn
// CHECK-NEXT:   unreachable

int test2() {
  try {
    A a, b;
    s();
    try {
      B f, g;
      s();
    } catch (B &) {}
  } catch (A &) {}
  return 0;
}

// CHECK: define i32 @"\01?test2@@YAHXZ"() {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %try.id = alloca i32
// CHECK:        store i32 -1, i32* %try.id
// CHECK-NEXT:   store i32 0, i32* %try.id
// CHECK-NEXT:   %call = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %a)

// CHECK:        store i32 1, i32* %try.id
// CHECK-NEXT:   %call2 = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %b)

// CHECK:        store i32 2, i32* %try.id
// CHECK-NEXT:   call void @"\01?s@@YAXXZ"()
// CHECK-NEXT:   store i32 3, i32* %try.id
// CHECK-NEXT:   %call4 = call x86_thiscallcc %struct.B* @"\01??0B@@QAE@XZ"(%struct.B* %f)

// CHECK:        store i32 4, i32* %try.id
// CHECK-NEXT:   %call6 = call x86_thiscallcc %struct.B* @"\01??0B@@QAE@XZ"(%struct.B* %g)

// CHECK:        store i32 5, i32* %try.id
// CHECK-NEXT:   call void @"\01?s@@YAXXZ"()
// CHECK-NEXT:   store i32 4, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1B@@QAE@XZ"(%struct.B* %g)
// CHECK-NEXT:   store i32 3, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1B@@QAE@XZ"(%struct.B* %f)

// CHECK: "\01__catch$test2@@YAHXZ$0":                      ; No predecessors!
// CHECK-NEXT:   store i32 2, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test2@@YAHXZ$1":                    
// CHECK-NEXT:   store i32 2, i32* %try.id
// CHECK-NEXT:   store i32 1, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   store i32 0, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__tryend$test2@@YAHXZ$3"

// CHECK: "\01__catch$test2@@YAHXZ$2":                      ; No predecessors!
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test2@@YAHXZ$3":                     ; preds = %"\01__tryend$test2@@YAHXZ$1"
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   ret i32 0

// CHECK: "\01__unwindfunclet$test2@@YAHXZ$4":              ; No predecessors!
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test2@@YAHXZ$5"

// CHECK: "\01__unwindfunclet$test2@@YAHXZ$5":              ; preds = %"\01__unwindfunclet$test2@@YAHXZ$4"
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test2@@YAHXZ$6"

// CHECK: "\01__unwindfunclet$test2@@YAHXZ$6":              ; preds = %"\01__unwindfunclet$test2@@YAHXZ$5"
// CHECK-NEXT:   call void @"\01??1B@@QAE@XZ"(%struct.B* %f)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test2@@YAHXZ$7"

// CHECK: "\01__unwindfunclet$test2@@YAHXZ$7":              ; preds = %"\01__unwindfunclet$test2@@YAHXZ$6"
// CHECK-NEXT:   call void @"\01??1B@@QAE@XZ"(%struct.B* %g)
// CHECK-NEXT:   call void @"\01__ehhandler$test2@@YAHXZ"() noreturn
// CHECK-NEXT:   unreachable

int test3() {
  try {
    A a, b;
    s();
  } catch (A &) {}
  
  try {
    B h, gh;
    s();
  } catch (B &) {}

  return 0;
}

// CHECK: define i32 @"\01?test3@@YAHXZ"() {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %try.id = alloca i32
// CHECK:   store i32 -1, i32* %try.id
// CHECK-NEXT:   store i32 0, i32* %try.id
// CHECK-NEXT:   %call = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %call.cont

// CHECK: call.cont:                                        ; preds = %entry
// CHECK-NEXT:   store i32 1, i32* %try.id
// CHECK-NEXT:   %call2 = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   br label %call.cont1

// CHECK: call.cont1:                                       ; preds = %call.cont
// CHECK-NEXT:   store i32 2, i32* %try.id
// CHECK-NEXT:   call void @"\01?s@@YAXXZ"()
// CHECK-NEXT:   store i32 1, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   store i32 0, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__tryend$test3@@YAHXZ$1"

// CHECK: "\01__catch$test3@@YAHXZ$0":                      ; No predecessors!
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test3@@YAHXZ$1":                     ; preds = %call.cont1
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   store i32 4, i32* %try.id
// CHECK-NEXT:   %call4 = call x86_thiscallcc %struct.B* @"\01??0B@@QAE@XZ"(%struct.B* %h)
// CHECK-NEXT:   br label %call.cont3

// CHECK: call.cont3:                                       ; preds = %"\01__tryend$test3@@YAHXZ$1"
// CHECK-NEXT:   store i32 5, i32* %try.id
// CHECK-NEXT:   %call6 = call x86_thiscallcc %struct.B* @"\01??0B@@QAE@XZ"(%struct.B* %gh)
// CHECK-NEXT:   br label %call.cont5

// CHECK: call.cont5:                                       ; preds = %call.cont3
// CHECK-NEXT:   store i32 6, i32* %try.id
// CHECK-NEXT:   call void @"\01?s@@YAXXZ"()
// CHECK-NEXT:   store i32 5, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1B@@QAE@XZ"(%struct.B* %gh)
// CHECK-NEXT:   store i32 4, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1B@@QAE@XZ"(%struct.B* %h)
// CHECK-NEXT:   br label %"\01__tryend$test3@@YAHXZ$3"

// CHECK: "\01__catch$test3@@YAHXZ$2":                      ; No predecessors!
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test3@@YAHXZ$3":                     ; preds = %call.cont5
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   ret i32 0

// CHECK: "\01__unwindfunclet$test3@@YAHXZ$4":              ; No predecessors!
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test3@@YAHXZ$5"

// CHECK: "\01__unwindfunclet$test3@@YAHXZ$5":              ; preds = %"\01__unwindfunclet$test3@@YAHXZ$4"
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test3@@YAHXZ$6"

// CHECK: "\01__unwindfunclet$test3@@YAHXZ$6":              ; preds = %"\01__unwindfunclet$test3@@YAHXZ$5"
// CHECK-NEXT:   call void @"\01??1B@@QAE@XZ"(%struct.B* %h)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test3@@YAHXZ$7"

// CHECK: "\01__unwindfunclet$test3@@YAHXZ$7":              ; preds = %"\01__unwindfunclet$test3@@YAHXZ$6"
// CHECK-NEXT:   call void @"\01??1B@@QAE@XZ"(%struct.B* %gh)
// CHECK-NEXT:   call void @"\01__ehhandler$test3@@YAHXZ"() noreturn
// CHECK-NEXT:   unreachable

int test4() {
  try {
    C fl;
    try {
      A a, b;
      s();
    } catch (A &) {}
    C gl;
    try {
      B h, gh;
      s();
    } catch (B &) {}
  } catch (C&) {}
  return 0;
}

// CHECK: define i32 @"\01?test4@@YAHXZ"() {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %try.id = alloca i32
// CHECK:   store i32 -1, i32* %try.id
// CHECK-NEXT:   store i32 0, i32* %try.id
// CHECK-NEXT:   %call = call x86_thiscallcc %struct.C* @"\01??0C@@QAE@XZ"(%struct.C* %fl)
// CHECK-NEXT:   br label %call.cont

// CHECK: call.cont:                                        ; preds = %entry
// CHECK-NEXT:   store i32 1, i32* %try.id
// CHECK-NEXT:   store i32 2, i32* %try.id
// CHECK-NEXT:   %call2 = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %call.cont1

// CHECK: call.cont1:                                       ; preds = %call.cont
// CHECK-NEXT:   store i32 3, i32* %try.id
// CHECK-NEXT:   %call4 = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   br label %call.cont3

// CHECK: call.cont3:                                       ; preds = %call.cont1
// CHECK-NEXT:   store i32 4, i32* %try.id
// CHECK-NEXT:   call void @"\01?s@@YAXXZ"()
// CHECK-NEXT:   store i32 3, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   store i32 2, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__tryend$test4@@YAHXZ$1"

// CHECK: "\01__catch$test4@@YAHXZ$0":                      ; No predecessors!
// CHECK-NEXT:   store i32 1, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test4@@YAHXZ$1":                     ; preds = %call.cont3
// CHECK-NEXT:   store i32 1, i32* %try.id
// CHECK-NEXT:   %call6 = call x86_thiscallcc %struct.C* @"\01??0C@@QAE@XZ"(%struct.C* %gl)
// CHECK-NEXT:   br label %call.cont5

// CHECK: call.cont5:                                       ; preds = %"\01__tryend$test4@@YAHXZ$1"
// CHECK-NEXT:   store i32 6, i32* %try.id
// CHECK-NEXT:   store i32 7, i32* %try.id
// CHECK-NEXT:   %call8 = call x86_thiscallcc %struct.B* @"\01??0B@@QAE@XZ"(%struct.B* %h)
// CHECK-NEXT:   br label %call.cont7

// CHECK: call.cont7:                                       ; preds = %call.cont5
// CHECK-NEXT:   store i32 8, i32* %try.id
// CHECK-NEXT:   %call10 = call x86_thiscallcc %struct.B* @"\01??0B@@QAE@XZ"(%struct.B* %gh)
// CHECK-NEXT:   br label %call.cont9

// CHECK: call.cont9:                                       ; preds = %call.cont7
// CHECK-NEXT:   store i32 9, i32* %try.id
// CHECK-NEXT:   call void @"\01?s@@YAXXZ"()
// CHECK-NEXT:   store i32 8, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1B@@QAE@XZ"(%struct.B* %gh)
// CHECK-NEXT:   store i32 7, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1B@@QAE@XZ"(%struct.B* %h)
// CHECK-NEXT:   br label %"\01__tryend$test4@@YAHXZ$3"

// CHECK: "\01__catch$test4@@YAHXZ$2":                      ; No predecessors!
// CHECK-NEXT:   store i32 6, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test4@@YAHXZ$3":                     ; preds = %call.cont9
// CHECK-NEXT:   store i32 6, i32* %try.id
// CHECK-NEXT:   store i32 1, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1C@@QAE@XZ"(%struct.C* %gl)
// CHECK-NEXT:   store i32 0, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1C@@QAE@XZ"(%struct.C* %fl)
// CHECK-NEXT:   br label %"\01__tryend$test4@@YAHXZ$5"

// CHECK: "\01__catch$test4@@YAHXZ$4":                      ; No predecessors!
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test4@@YAHXZ$5":                     ; preds = %"\01__tryend$test4@@YAHXZ$3"
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   ret i32 0

// CHECK: "\01__unwindfunclet$test4@@YAHXZ$6":              ; No predecessors!
// CHECK-NEXT:   call void @"\01??1C@@QAE@XZ"(%struct.C* %fl)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test4@@YAHXZ$7"

// CHECK: "\01__unwindfunclet$test4@@YAHXZ$7":              ; preds = %"\01__unwindfunclet$test4@@YAHXZ$6"
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test4@@YAHXZ$8"

// CHECK: "\01__unwindfunclet$test4@@YAHXZ$8":              ; preds = %"\01__unwindfunclet$test4@@YAHXZ$7"
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test4@@YAHXZ$9"

// CHECK: "\01__unwindfunclet$test4@@YAHXZ$9":              ; preds = %"\01__unwindfunclet$test4@@YAHXZ$8"
// CHECK-NEXT:   call void @"\01??1C@@QAE@XZ"(%struct.C* %gl)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test4@@YAHXZ$10"

// CHECK: "\01__unwindfunclet$test4@@YAHXZ$10":             ; preds = %"\01__unwindfunclet$test4@@YAHXZ$9"
// CHECK-NEXT:   call void @"\01??1B@@QAE@XZ"(%struct.B* %h)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test4@@YAHXZ$11"

// CHECK: "\01__unwindfunclet$test4@@YAHXZ$11":             ; preds = %"\01__unwindfunclet$test4@@YAHXZ$10"
// CHECK-NEXT:   call void @"\01??1B@@QAE@XZ"(%struct.B* %gh)
// CHECK-NEXT:   call void @"\01__ehhandler$test4@@YAHXZ"() noreturn
// CHECK-NEXT:   unreachable

int test5() {
  C ad;
  try {
    C fl;
    try {
      A a, b;
      s();
    } catch (A &) {}
    C gl;
    try {
      B h, gh;
      s();
    } catch (B &) {}
  } catch (C&) {}
  s();
  return 0;
}

// CHECK: define i32 @"\01?test5@@YAHXZ"() {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %try.id = alloca i32
// CHECK:   store i32 -1, i32* %try.id
// CHECK-NEXT:   %call = call x86_thiscallcc %struct.C* @"\01??0C@@QAE@XZ"(%struct.C* %ad)
// CHECK-NEXT:   br label %call.cont

// CHECK: call.cont:                                        ; preds = %entry
// CHECK-NEXT:   store i32 0, i32* %try.id
// CHECK-NEXT:   store i32 1, i32* %try.id
// CHECK-NEXT:   %call2 = call x86_thiscallcc %struct.C* @"\01??0C@@QAE@XZ"(%struct.C* %fl)
// CHECK-NEXT:   br label %call.cont1

// CHECK: call.cont1:                                       ; preds = %call.cont
// CHECK-NEXT:   store i32 2, i32* %try.id
// CHECK-NEXT:   store i32 3, i32* %try.id
// CHECK-NEXT:   %call4 = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %call.cont3

// CHECK: call.cont3:                                       ; preds = %call.cont1
// CHECK-NEXT:   store i32 4, i32* %try.id
// CHECK-NEXT:   %call6 = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   br label %call.cont5

// CHECK: call.cont5:                                       ; preds = %call.cont3
// CHECK-NEXT:   store i32 5, i32* %try.id
// CHECK-NEXT:   call void @"\01?s@@YAXXZ"()
// CHECK-NEXT:   store i32 4, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   store i32 3, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__tryend$test5@@YAHXZ$1"

// CHECK: "\01__catch$test5@@YAHXZ$0":                      ; No predecessors!
// CHECK-NEXT:   store i32 2, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test5@@YAHXZ$1":                     ; preds = %call.cont5
// CHECK-NEXT:   store i32 2, i32* %try.id
// CHECK-NEXT:   %call8 = call x86_thiscallcc %struct.C* @"\01??0C@@QAE@XZ"(%struct.C* %gl)
// CHECK-NEXT:   br label %call.cont7

// CHECK: call.cont7:                                       ; preds = %"\01__tryend$test5@@YAHXZ$1"
// CHECK-NEXT:   store i32 7, i32* %try.id
// CHECK-NEXT:   store i32 8, i32* %try.id
// CHECK-NEXT:   %call10 = call x86_thiscallcc %struct.B* @"\01??0B@@QAE@XZ"(%struct.B* %h)
// CHECK-NEXT:   br label %call.cont9

// CHECK: call.cont9:                                       ; preds = %call.cont7
// CHECK-NEXT:   store i32 9, i32* %try.id
// CHECK-NEXT:   %call12 = call x86_thiscallcc %struct.B* @"\01??0B@@QAE@XZ"(%struct.B* %gh)
// CHECK-NEXT:   br label %call.cont11

// CHECK: call.cont11:                                      ; preds = %call.cont9
// CHECK-NEXT:   store i32 10, i32* %try.id
// CHECK-NEXT:   call void @"\01?s@@YAXXZ"()
// CHECK-NEXT:   store i32 9, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1B@@QAE@XZ"(%struct.B* %gh)
// CHECK-NEXT:   store i32 8, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1B@@QAE@XZ"(%struct.B* %h)
// CHECK-NEXT:   br label %"\01__tryend$test5@@YAHXZ$3"

// CHECK: "\01__catch$test5@@YAHXZ$2":                      ; No predecessors!
// CHECK-NEXT:   store i32 7, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test5@@YAHXZ$3":                     ; preds = %call.cont11
// CHECK-NEXT:   store i32 7, i32* %try.id
// CHECK-NEXT:   store i32 2, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1C@@QAE@XZ"(%struct.C* %gl)
// CHECK-NEXT:   store i32 1, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1C@@QAE@XZ"(%struct.C* %fl)
// CHECK-NEXT:   br label %"\01__tryend$test5@@YAHXZ$5"

// CHECK: "\01__catch$test5@@YAHXZ$4":                      ; No predecessors!
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test5@@YAHXZ$5":                     ; preds = %"\01__tryend$test5@@YAHXZ$3"
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   call void @"\01?s@@YAXXZ"()
// CHECK-NEXT:   store i32 1, i32* %cleanup.dest.slot
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1C@@QAE@XZ"(%struct.C* %ad)
// CHECK-NEXT:   ret i32 0

// CHECK: "\01__unwindfunclet$test5@@YAHXZ$6":              ; No predecessors!
// CHECK-NEXT:   call void @"\01??1C@@QAE@XZ"(%struct.C* %ad)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test5@@YAHXZ$7"

// CHECK: "\01__unwindfunclet$test5@@YAHXZ$7":              ; preds = %"\01__unwindfunclet$test5@@YAHXZ$6"
// CHECK-NEXT:   call void @"\01??1C@@QAE@XZ"(%struct.C* %fl)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test5@@YAHXZ$8"

// CHECK: "\01__unwindfunclet$test5@@YAHXZ$8":              ; preds = %"\01__unwindfunclet$test5@@YAHXZ$7"
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test5@@YAHXZ$9"

// CHECK: "\01__unwindfunclet$test5@@YAHXZ$9":              ; preds = %"\01__unwindfunclet$test5@@YAHXZ$8"
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test5@@YAHXZ$10"

// CHECK: "\01__unwindfunclet$test5@@YAHXZ$10":             ; preds = %"\01__unwindfunclet$test5@@YAHXZ$9"
// CHECK-NEXT:   call void @"\01??1C@@QAE@XZ"(%struct.C* %gl)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test5@@YAHXZ$11"

// CHECK: "\01__unwindfunclet$test5@@YAHXZ$11":             ; preds = %"\01__unwindfunclet$test5@@YAHXZ$10"
// CHECK-NEXT:   call void @"\01??1B@@QAE@XZ"(%struct.B* %h)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test5@@YAHXZ$12"

// CHECK: "\01__unwindfunclet$test5@@YAHXZ$12":             ; preds = %"\01__unwindfunclet$test5@@YAHXZ$11"
// CHECK-NEXT:   call void @"\01??1B@@QAE@XZ"(%struct.B* %gh)
// CHECK-NEXT:   call void @"\01__ehhandler$test5@@YAHXZ"() noreturn
// CHECK-NEXT:   unreachable