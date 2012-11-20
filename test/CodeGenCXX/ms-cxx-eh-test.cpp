// RUN: %clang_cc1  -emit-llvm %s -o - -cxx-abi microsoft -fcxx-exceptions -fexceptions -triple=i686-pc-win32-microsoft | FileCheck %s
// r4start

//////////////////////////////////////////////Test 1//////////////////////////////////////////////

// CHECK: @"\01__CT??_R0H@84" = weak global %catchable.type { i32 1, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), %pmd.type { i32 0, i32 -1, i32 0 }, i32 4, void ()* null }

// CHECK: @"\01__CTA1H" = weak global { i32, [1 x %catchable.type*] } { i32 1, [1 x %catchable.type*] [%catchable.type* @"\01__CT??_R0H@84"] }

// CHECK: @"\01__TI1H" = weak global %throw.info.type { i32 0, void ()* null, i32 ()* null, %catchable.array.type* bitcast ({ i32, [1 x %catchable.type*] }* @"\01__CTA1H" to %catchable.array.type*) }

// CHECK: @"\01__catchsym$test1@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test1@@YAHXZ", %catch) }]

// CHECK: @"\01__tryblocktable$test1@@YAHXZ" = weak global [1 x %tryblock.map.entry] [%tryblock.map.entry { i32 0, i32 2, i32 3, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test1@@YAHXZ$0", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test1@@YAHXZ" = weak global [4 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test1@@YAHXZ", %ehcleanup) }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test1@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 4, %unwind.map.entry* getelementptr inbounds ([4 x %unwind.map.entry]* @"\01__unwindtable$test1@@YAHXZ", i32 0, i32 0), i32 1, %tryblock.map.entry* getelementptr inbounds ([1 x %tryblock.map.entry]* @"\01__tryblocktable$test1@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////Test 2//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test2@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUB@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test2@@YAHXZ", %catch) }]

// CHECK: @"\01__catchsym$test2@@YAHXZ$1" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test2@@YAHXZ", %catch5) }]

// CHECK: @"\01__tryblocktable$test2@@YAHXZ" = weak global [2 x %tryblock.map.entry] [%tryblock.map.entry { i32 1, i32 1, i32 2, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test2@@YAHXZ$0", i32 0, i32 0) }, %tryblock.map.entry { i32 0, i32 2, i32 3, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test2@@YAHXZ$1", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test2@@YAHXZ" = weak global [4 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry zeroinitializer, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test2@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 4, %unwind.map.entry* getelementptr inbounds ([4 x %unwind.map.entry]* @"\01__unwindtable$test2@@YAHXZ", i32 0, i32 0), i32 2, %tryblock.map.entry* getelementptr inbounds ([2 x %tryblock.map.entry]* @"\01__tryblocktable$test2@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////Test 3//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test3@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test3@@YAHXZ", %catch) }]

// CHECK: @"\01__catchsym$test3@@YAHXZ$1" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test3@@YAHXZ", %catch5) }]

// CHECK: @"\01__tryblocktable$test3@@YAHXZ" = weak global [2 x %tryblock.map.entry] [%tryblock.map.entry { i32 0, i32 0, i32 1, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test3@@YAHXZ$0", i32 0, i32 0) }, %tryblock.map.entry { i32 2, i32 2, i32 3, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test3@@YAHXZ$1", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test3@@YAHXZ" = weak global [4 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test3@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 4, %unwind.map.entry* getelementptr inbounds ([4 x %unwind.map.entry]* @"\01__unwindtable$test3@@YAHXZ", i32 0, i32 0), i32 2, %tryblock.map.entry* getelementptr inbounds ([2 x %tryblock.map.entry]* @"\01__tryblocktable$test3@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////Test 4//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test4@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test4@@YAHXZ", %catch) }]

// CHECK: @"\01__catchsym$test4@@YAHXZ$1" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test4@@YAHXZ", %catch5) }]

// CHECK: @"\01__catchsym$test4@@YAHXZ$2" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUC@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test4@@YAHXZ", %catch8) }]

// CHECK: @"\01__tryblocktable$test4@@YAHXZ" = weak global [3 x %tryblock.map.entry] [%tryblock.map.entry { i32 1, i32 1, i32 2, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test4@@YAHXZ$0", i32 0, i32 0) }, %tryblock.map.entry { i32 3, i32 3, i32 4, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test4@@YAHXZ$1", i32 0, i32 0) }, %tryblock.map.entry { i32 0, i32 4, i32 5, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test4@@YAHXZ$2", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test4@@YAHXZ" = weak global [6 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry zeroinitializer, %unwind.map.entry zeroinitializer, %unwind.map.entry zeroinitializer, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test4@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 6, %unwind.map.entry* getelementptr inbounds ([6 x %unwind.map.entry]* @"\01__unwindtable$test4@@YAHXZ", i32 0, i32 0), i32 3, %tryblock.map.entry* getelementptr inbounds ([3 x %tryblock.map.entry]* @"\01__tryblocktable$test4@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////Test 5//////////////////////////////////////////////

// : @"\01__catchsym$test5@@YAHXZ$2" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__catch$test5@@YAHXZ$0") }]

// : @"\01__catchsym$test5@@YAHXZ$5" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUB@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__catch$test5@@YAHXZ$3") }]

// : @"\01__catchsym$test5@@YAHXZ$8" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUC@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__catch$test5@@YAHXZ$6") }]

// : @"\01__tryblocktable$test5@@YAHXZ" = internal constant [3 x %tryblock.map.entry] [%tryblock.map.entry { i32 3, i32 5, i32 6, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test5@@YAHXZ$2", i32 0, i32 0) }, %tryblock.map.entry { i32 8, i32 10, i32 11, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test5@@YAHXZ$5", i32 0, i32 0) }, %tryblock.map.entry { i32 1, i32 11, i32 12, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test5@@YAHXZ$8", i32 0, i32 0) }]

// : @"\01__unwindtable$test5@@YAHXZ" = internal constant [13 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$9") }, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$10") }, %unwind.map.entry { i32 2, i8* null }, %unwind.map.entry { i32 3, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$11") }, %unwind.map.entry { i32 4, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$12") }, %unwind.map.entry { i32 2, i8* null }, %unwind.map.entry { i32 2, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$13") }, %unwind.map.entry { i32 7, i8* null }, %unwind.map.entry { i32 8, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$14") }, %unwind.map.entry { i32 9, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$15") }, %unwind.map.entry { i32 7, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

//////////////////////////////////////////////Test 6//////////////////////////////////////////////

// : @"\01__catchsym$test6@@YAHXZ$3" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUC@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__catch$test6@@YAHXZ$1") }]

// : @"\01__catchsym$test6@@YAHXZ$5" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__catch$test6@@YAHXZ$0") }]

// : @"\01__catchsym$test6@@YAHXZ$8" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUB@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__catch$test6@@YAHXZ$6") }]

// : @"\01__catchsym$test6@@YAHXZ$11" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUC@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__catch$test6@@YAHXZ$9") }]

// : @"\01__tryblocktable$test6@@YAHXZ" = internal constant [4 x %tryblock.map.entry] [%tryblock.map.entry { i32 9, i32 9, i32 10, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test6@@YAHXZ$3", i32 0, i32 0) }, %tryblock.map.entry { i32 4, i32 6, i32 7, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test6@@YAHXZ$5", i32 0, i32 0) }, %tryblock.map.entry { i32 13, i32 15, i32 16, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test6@@YAHXZ$8", i32 0, i32 0) }, %tryblock.map.entry { i32 2, i32 16, i32 17, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test6@@YAHXZ$11", i32 0, i32 0) }]

// : @"\01__unwindtable$test6@@YAHXZ" = internal constant [19 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$12") }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$13") }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 2, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$14") }, %unwind.map.entry { i32 3, i8* null }, %unwind.map.entry { i32 4, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$15") }, %unwind.map.entry { i32 5, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$16") }, %unwind.map.entry { i32 3, i8* null }, %unwind.map.entry { i32 3, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$17") }, %unwind.map.entry { i32 8, i8* null }, %unwind.map.entry { i32 8, i8* null }, %unwind.map.entry { i32 8, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$18") }, %unwind.map.entry { i32 11, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$19") }, %unwind.map.entry { i32 12, i8* null }, %unwind.map.entry { i32 13, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$20") }, %unwind.map.entry { i32 14, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$21") }, %unwind.map.entry { i32 12, i8* null }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$22") }]

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 10/////////////////////////////////////////////

// CHECK: @"\01__catchsym$test10@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 0, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test10@@YAHXZ", %catch) }]

// CHECK: @"\01__catchsym$test10@@YAHXZ$1" = weak global [1 x %handler.type] [%handler.type { i32 0, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test10@@YAHXZ", %catch10) }]

// CHECK: @"\01__catchsym$test10@@YAHXZ$2" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test10@@YAHXZ", %catch5) }]

// CHECK: @"\01__tryblocktable$test10@@YAHXZ" = weak global [3 x %tryblock.map.entry] [%tryblock.map.entry { i32 1, i32 1, i32 2, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test10@@YAHXZ$0", i32 0, i32 0) }, %tryblock.map.entry { i32 4, i32 4, i32 5, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test10@@YAHXZ$1", i32 0, i32 0) }, %tryblock.map.entry { i32 0, i32 2, i32 5, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test10@@YAHXZ$2", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test10@@YAHXZ" = weak global [6 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry zeroinitializer, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 3, i8* null }, %unwind.map.entry { i32 3, i8* null }]

// CHECK: @"\01__ehfuncinfo$test10@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 6, %unwind.map.entry* getelementptr inbounds ([6 x %unwind.map.entry]* @"\01__unwindtable$test10@@YAHXZ", i32 0, i32 0), i32 3, %tryblock.map.entry* getelementptr inbounds ([3 x %tryblock.map.entry]* @"\01__tryblocktable$test10@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 11/////////////////////////////////////////////

// CHECK: @"\01__catchsym$test11@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test11@@YAHXZ", %catch) }]
// CHECK: @"\01__tryblocktable$test11@@YAHXZ" = weak global [1 x %tryblock.map.entry] [%tryblock.map.entry { i32 0, i32 1, i32 2, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test11@@YAHXZ$0", i32 0, i32 0) }]
// CHECK: @"\01__unwindtable$test11@@YAHXZ" = weak global [3 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test11@@YAHXZ", %ehcleanup) }, %unwind.map.entry { i32 -1, i8* null }]
// CHECK: @"\01__ehfuncinfo$test11@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 3, %unwind.map.entry* getelementptr inbounds ([3 x %unwind.map.entry]* @"\01__unwindtable$test11@@YAHXZ", i32 0, i32 0), i32 1, %tryblock.map.entry* getelementptr inbounds ([1 x %tryblock.map.entry]* @"\01__tryblocktable$test11@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

struct A {
  A(){}
  ~A(){}
};


struct B {
  B(){}
  ~B(){}
};

struct C {
};

struct D{
  ~D(){}
};

void s() { throw 1; }

int test1() {
  try {
    A a, b;
  } catch (A &) {}
  return 0;
}

int test2() {
  try {
    s();
    try {
      s();
    } catch (B &) {}
  } catch (int &) {}
  return 0;
}

int test3() {
  try {
    s();
  } catch (int &) {
  }
  try {
    s();
  } catch (int &) {
  }
  return 0;
}

int test4() {
  try {
    C fl;
    try {
      C a, b;
      s();
    } catch (int &) {
    }
    C gl;
    try {
      C h, gh;
      s();
    } catch (int &) {
    }
  } catch (C&) {}
  return 0;
}

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

int test6() {
  C ad;
  B ghj;
  try {
    C fl;
    try {
      A a, b;
      s();
    } catch (A &) {
      A sd;
      try {
        s();
      } catch (C&) {}
      B j;
      s();
    }
    C gl;
    try {
      B h, gh;
      s();
    } catch (B &) {}
  } catch (C&) {}
  C a;
  s();
  return 0;
}

int test7() {
  try {
    s();
  } catch (A&) {

  }
  return 0;
}

int test8() {
  try {
    throw D();
  } catch (D&) {
  }
  return 0;
}

int test9() {
  try {
    D a;
    throw a;
  } catch (D&) {
  }
  return 0;
}

int test10() {
  try {
    try {
      s();
    } catch (int u) {
    }
    s();
  } catch (int &uga2) {
    try {
      s();
    } catch (int u) {
    }
  }
  return 0;
}

int test11() {
  try {
    A a;
    s();
  } catch (int &ex) {
  }
  return 0;
}
