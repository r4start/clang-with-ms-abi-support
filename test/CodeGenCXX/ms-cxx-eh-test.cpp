// RUN: %clang_cc1  -emit-llvm %s -o - -cxx-abi microsoft -fcxx-exceptions -fexceptions -triple=i686-pc-win32 | FileCheck %s
// r4start
struct my_exception{
};

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

struct Z {
 Z() { }
 ~Z() { }
};

struct X {
 X() { }
 ~X() { }
};

struct V {
 V() { }
 ~V() { }
};

struct S {
 S() { }
 ~S() { }
};

struct D {
 D() { }
 ~D() { }
};

struct F {
 F() { }
 ~F() { }
};

struct G {
 G() { }
 ~G() { }
};

int test() {
try {
    A a, b;
    try {
      int yy = 90999;
      yy -= 89;
    } catch (F&) {
    }
    int r = 90;
    r  -= 89;
    try {
    } catch (B&) {
    }
    int uy = 8;
    try {
    } catch (G&) {
    } catch (D&) {}
    catch (X&){}

    int asklfaslkd = 42;
    try { asklfaslkd = 16; }
    catch(S&) {}

    try { asklfaslkd = 19; } catch (my_exception &) {}

  } catch (my_exception &ex) {

  }
  catch (A &) {}
  catch (B &) {}
  catch (C &) {}
  catch (Z &) {}
  catch (X &) {}
  catch (V &) {}
  catch (S &) {}
  catch (D &) {}
  catch (F &) {}  
  catch (G &) {
  }
  return 0;
}

int test2() {
try {
    A a, b;
  } catch (A &) {}

  try {
    C d, f;
    A a, b;
  } catch (B &) {

  }
  return 0;
}

extern int s();

int test3() {
try {
    A a, b;
    s();
    try {
      s();
      B h;
    } catch (G &) {}
  } catch (A &) {}

  try {
    C d, f;
    A a, b;
  } catch (B &) { s();}

  return 0;
}

// CHECK: @"\01__unwindtable$test@@YAHXZ" = internal constant [13 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test@@YAHXZ", %"\01__unwindfunclet$test@@YAHXZ$24") }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__unwindtable$test2@@YAHXZ" = internal constant [8 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__unwindfunclet$test2@@YAHXZ$4") }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 3, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__unwindfunclet$test2@@YAHXZ$5") }, %unwind.map.entry { i32 4, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__unwindfunclet$test2@@YAHXZ$6") }, %unwind.map.entry { i32 5, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__unwindfunclet$test2@@YAHXZ$7") }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK:        define i32 @"\01?test@@YAHXZ"() {
// CHECK:        entry:
// CHECK:          %try.id = alloca i32
// CHECK:          store i32 -1, i32* %try.id
// CHECK-NEXT:     store i32 0, i32* %try.id
// CHECK-NEXT:     %call = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:     br label %call.cont

// CHECK:   call.cont:                                        ; preds = %entry
// CHECK-NEXT:     store i32 1, i32* %try.id
// CHECK-NEXT:     %call2 = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:     br label %call.cont1

// CHECK:   call.cont1:                                       ; preds = %call.cont
// CHECK-NEXT:     store i32 2, i32* %try.id
// CHECK:          br label %"\01__tryend$test@@YAHXZ$1"

// CHECK:       "\01__catch$test@@YAHXZ$0":                       ; No predecessors!
// CHECK:          store i32 1, i32* %try.id
// CHECK:          unreachable

// CHECK:       "\01__tryend$test@@YAHXZ$1":                      ; preds = %call.cont1
// CHECK:          store i32 1, i32* %try.id
// CHECK:          store i32 4, i32* %try.id
// CHECK-NEXT:     br label %"\01__tryend$test@@YAHXZ$3"

// CHECK:       "\01__catch$test@@YAHXZ$2":                       ; No predecessors!
// CHECK:          store i32 1, i32* %try.id
// CHECK:          unreachable

// CHECK:       "\01__tryend$test@@YAHXZ$3":                      ; preds = %"\01__tryend$test@@YAHXZ$1"
// CHECK-NEXT:    store i32 1, i32* %try.id
// CHECK:         store i32 6, i32* %try.id
// CHECK-NEXT:    br label %"\01__tryend$test@@YAHXZ$7"

// CHECK:       "\01__catch$test@@YAHXZ$4":                       ; No predecessors!
// CHECK-NEXT:     store i32 1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__catch$test@@YAHXZ$5":                       ; No predecessors!
// CHECK-NEXT:     store i32 1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__catch$test@@YAHXZ$6":                       ; No predecessors!
// CHECK-NEXT:     store i32 1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__tryend$test@@YAHXZ$7":                      ; preds = %"\01__tryend$test@@YAHXZ$3"
// CHECK-NEXT:     store i32 1, i32* %try.id
// CHECK:          store i32 8, i32* %try.id
// CHECK:          br label %"\01__tryend$test@@YAHXZ$9"

// CHECK:        "\01__catch$test@@YAHXZ$8":                       ; No predecessors!
// CHECK-NEXT:     store i32 1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__tryend$test@@YAHXZ$9":                      ; preds = %"\01__tryend$test@@YAHXZ$7"
// CHECK-NEXT:     store i32 1, i32* %try.id
// CHECK:          store i32 10, i32* %try.id
// CHECK:          br label %"\01__tryend$test@@YAHXZ$11"

// CHECK:        "\01__catch$test@@YAHXZ$10":                      ; No predecessors!
// CHECK-NEXT:     store i32 1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__tryend$test@@YAHXZ$11":                     ; preds = %"\01__tryend$test@@YAHXZ$9"
// CHECK-NEXT:     store i32 1, i32* %try.id
// CHECK-NEXT:     call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:     store i32 0, i32* %try.id
// CHECK-NEXT:     call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:     br label %"\01__tryend$test@@YAHXZ$23"

// CHECK:        "\01__catch$test@@YAHXZ$12":                      ; No predecessors!
// CHECK-NEXT:     store i32 -1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__catch$test@@YAHXZ$13":                      ; No predecessors!
// CHECK-NEXT:     store i32 -1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__catch$test@@YAHXZ$14":                      ; No predecessors!
// CHECK-NEXT:     store i32 -1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__catch$test@@YAHXZ$15":                      ; No predecessors!
// CHECK-NEXT:     store i32 -1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__catch$test@@YAHXZ$16":                      ; No predecessors!
// CHECK-NEXT:     store i32 -1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__catch$test@@YAHXZ$17":                      ; No predecessors!
// CHECK-NEXT:     store i32 -1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__catch$test@@YAHXZ$18":                      ; No predecessors!
// CHECK-NEXT:     store i32 -1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__catch$test@@YAHXZ$19":                      ; No predecessors!
// CHECK-NEXT:     store i32 -1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__catch$test@@YAHXZ$20":                      ; No predecessors!
// CHECK-NEXT:     store i32 -1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__catch$test@@YAHXZ$21":                      ; No predecessors!
// CHECK-NEXT:     store i32 -1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__catch$test@@YAHXZ$22":                      ; No predecessors!
// CHECK-NEXT:     store i32 -1, i32* %try.id
// CHECK-NEXT:     unreachable

// CHECK:        "\01__tryend$test@@YAHXZ$23":                     ; preds = %"\01__tryend$test@@YAHXZ$11"
// CHECK-NEXT:     store i32 -1, i32* %try.id
// CHECK-NEXT:     ret i32 0

// CHECK:        "\01__unwindfunclet$test@@YAHXZ$24":              ; No predecessors!
// CHECK-NEXT:     call void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:     call void @"\01__ehhandler$test@@YAHXZ"() noreturn
// CHECK-NEXT:     unreachable

// CHECK: define i32 @"\01?test2@@YAHXZ"() {
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
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %b)
// CHECK-NEXT:   store i32 0, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__tryend$test2@@YAHXZ$1"

// CHECK: "\01__catch$test2@@YAHXZ$0":                      ; No predecessors!
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test2@@YAHXZ$1":                     ; preds = %call.cont1
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   store i32 3, i32* %try.id
// CHECK-NEXT:   %call4 = call x86_thiscallcc %struct.C* @"\01??0C@@QAE@XZ"(%struct.C* %d)
// CHECK-NEXT:   br label %call.cont3

// CHECK: call.cont3:                                       ; preds = %"\01__tryend$test2@@YAHXZ$1"
// CHECK-NEXT:   store i32 4, i32* %try.id
// CHECK-NEXT:   %call6 = call x86_thiscallcc %struct.C* @"\01??0C@@QAE@XZ"(%struct.C* %f)
// CHECK-NEXT:   br label %call.cont5

// CHECK: call.cont5:                                       ; preds = %call.cont3
// CHECK-NEXT:   store i32 5, i32* %try.id
// CHECK-NEXT:   %call9 = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %a7)
// CHECK-NEXT:   br label %call.cont8

// CHECK: call.cont8:                                       ; preds = %call.cont5
// CHECK-NEXT:   store i32 6, i32* %try.id
// CHECK-NEXT:   %call12 = call x86_thiscallcc %struct.A* @"\01??0A@@QAE@XZ"(%struct.A* %b10)
// CHECK-NEXT:   br label %call.cont11

// CHECK: call.cont11:                                      ; preds = %call.cont8
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %b10)
// CHECK-NEXT:   store i32 5, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1A@@QAE@XZ"(%struct.A* %a7)
// CHECK-NEXT:   store i32 4, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1C@@QAE@XZ"(%struct.C* %f)
// CHECK-NEXT:   store i32 3, i32* %try.id
// CHECK-NEXT:   call x86_thiscallcc void @"\01??1C@@QAE@XZ"(%struct.C* %d)
// CHECK-NEXT:   br label %"\01__tryend$test2@@YAHXZ$3"

// CHECK: "\01__catch$test2@@YAHXZ$2":                      ; No predecessors!
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   unreachable

// CHECK: "\01__tryend$test2@@YAHXZ$3":                     ; preds = %call.cont11
// CHECK-NEXT:   store i32 -1, i32* %try.id
// CHECK-NEXT:   ret i32 0

// CHECK: "\01__unwindfunclet$test2@@YAHXZ$4":              ; No predecessors!
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %a)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test2@@YAHXZ$5"

// TODO: Fix "\01__unwindfunclet$test2@@YAHXZ$6"
// CHECK: "\01__unwindfunclet$test2@@YAHXZ$5":              ; preds = %"\01__unwindfunclet$test2@@YAHXZ$4"
// CHECK-NEXT:   call void @"\01??1C@@QAE@XZ"(%struct.C* %d)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test2@@YAHXZ$6"

// TODO: Fix "\01__unwindfunclet$test2@@YAHXZ$7"
// CHECK: "\01__unwindfunclet$test2@@YAHXZ$6":              ; preds = %"\01__unwindfunclet$test2@@YAHXZ$5"
// CHECK-NEXT:   call void @"\01??1C@@QAE@XZ"(%struct.C* %f)
// CHECK-NEXT:   br label %"\01__unwindfunclet$test2@@YAHXZ$7"

// TODO: Fix "\01__unwindfunclet$test2@@YAHXZ$8"
// CHECK: "\01__unwindfunclet$test2@@YAHXZ$7":              ; preds = %"\01__unwindfunclet$test2@@YAHXZ$6"
// CHECK-NEXT:   call void @"\01??1A@@QAE@XZ"(%struct.A* %a7)
// CHECK-NEXT:   call void @"\01__ehhandler$test2@@YAHXZ"() noreturn
// CHECK-NEXT:   unreachable

