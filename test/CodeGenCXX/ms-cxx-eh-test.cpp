// RUN: %clang_cc1  -emit-llvm %s -o - -cxx-abi microsoft -fcxx-exceptions -fexceptions -triple=i686-pc-win32 | FileCheck %s
// r4start

//////////////////////////////////////////////Test 1//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test1@@YAHXZ$2" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test1@@YAHXZ", %"\01__catch$test1@@YAHXZ$0") }]

// CHECK: @"\01__tryblocktable$test1@@YAHXZ" = internal constant [1 x %tryblock.map.entry] [%tryblock.map.entry { i32 0, i32 2, i32 3, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test1@@YAHXZ$2", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test1@@YAHXZ" = internal constant [4 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test1@@YAHXZ", %"\01__unwindfunclet$test1@@YAHXZ$3") }, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test1@@YAHXZ", %"\01__unwindfunclet$test1@@YAHXZ$4") }, %unwind.map.entry { i32 -1, i8* null }]

//////////////////////////////////////////////Test 2//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test2@@YAHXZ$2" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUB@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__catch$test2@@YAHXZ$0") }]

// CHECK: @"\01__catchsym$test2@@YAHXZ$5" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__catch$test2@@YAHXZ$3") }]

// CHECK: @"\01__tryblocktable$test2@@YAHXZ" = internal constant [2 x %tryblock.map.entry] [%tryblock.map.entry { i32 3, i32 5, i32 6, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test2@@YAHXZ$2", i32 0, i32 0) }, %tryblock.map.entry { i32 0, i32 6, i32 7, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test2@@YAHXZ$5", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test2@@YAHXZ" = internal constant [8 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__unwindfunclet$test2@@YAHXZ$6") }, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__unwindfunclet$test2@@YAHXZ$7") }, %unwind.map.entry { i32 2, i8* null }, %unwind.map.entry { i32 3, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__unwindfunclet$test2@@YAHXZ$8") }, %unwind.map.entry { i32 4, i8* blockaddress(@"\01?test2@@YAHXZ", %"\01__unwindfunclet$test2@@YAHXZ$9") }, %unwind.map.entry { i32 2, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

//////////////////////////////////////////////Test 3//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test3@@YAHXZ$2" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test3@@YAHXZ", %"\01__catch$test3@@YAHXZ$0") }]

// CHECK: @"\01__catchsym$test3@@YAHXZ$5" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUB@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test3@@YAHXZ", %"\01__catch$test3@@YAHXZ$3") }]

// CHECK: @"\01__tryblocktable$test3@@YAHXZ" = internal constant [2 x %tryblock.map.entry] [%tryblock.map.entry { i32 0, i32 2, i32 3, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test3@@YAHXZ$2", i32 0, i32 0) }, %tryblock.map.entry { i32 4, i32 6, i32 7, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test3@@YAHXZ$5", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test3@@YAHXZ" = internal constant [8 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test3@@YAHXZ", %"\01__unwindfunclet$test3@@YAHXZ$6") }, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test3@@YAHXZ", %"\01__unwindfunclet$test3@@YAHXZ$7") }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 4, i8* blockaddress(@"\01?test3@@YAHXZ", %"\01__unwindfunclet$test3@@YAHXZ$8") }, %unwind.map.entry { i32 5, i8* blockaddress(@"\01?test3@@YAHXZ", %"\01__unwindfunclet$test3@@YAHXZ$9") }, %unwind.map.entry { i32 -1, i8* null }]

//////////////////////////////////////////////Test 4//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test4@@YAHXZ$2" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__catch$test4@@YAHXZ$0") }]

// CHECK: @"\01__catchsym$test4@@YAHXZ$5" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUB@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__catch$test4@@YAHXZ$3") }]

// CHECK: @"\01__catchsym$test4@@YAHXZ$8" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUC@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__catch$test4@@YAHXZ$6") }]

// CHECK: @"\01__tryblocktable$test4@@YAHXZ" = internal constant [3 x %tryblock.map.entry] [%tryblock.map.entry { i32 2, i32 4, i32 5, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test4@@YAHXZ$2", i32 0, i32 0) }, %tryblock.map.entry { i32 7, i32 9, i32 10, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test4@@YAHXZ$5", i32 0, i32 0) }, %tryblock.map.entry { i32 0, i32 10, i32 11, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test4@@YAHXZ$8", i32 0, i32 0) }]


// CHECK: @"\01__unwindtable$test4@@YAHXZ" = internal constant [12 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__unwindfunclet$test4@@YAHXZ$9") }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 2, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__unwindfunclet$test4@@YAHXZ$10") }, %unwind.map.entry { i32 3, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__unwindfunclet$test4@@YAHXZ$11") }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__unwindfunclet$test4@@YAHXZ$12") }, %unwind.map.entry { i32 6, i8* null }, %unwind.map.entry { i32 7, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__unwindfunclet$test4@@YAHXZ$13") }, %unwind.map.entry { i32 8, i8* blockaddress(@"\01?test4@@YAHXZ", %"\01__unwindfunclet$test4@@YAHXZ$14") }, %unwind.map.entry { i32 6, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

//////////////////////////////////////////////Test 5//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test5@@YAHXZ$2" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__catch$test5@@YAHXZ$0") }]

// CHECK: @"\01__catchsym$test5@@YAHXZ$5" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUB@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__catch$test5@@YAHXZ$3") }]

// CHECK: @"\01__catchsym$test5@@YAHXZ$8" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUC@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__catch$test5@@YAHXZ$6") }]

// CHECK: @"\01__tryblocktable$test5@@YAHXZ" = internal constant [3 x %tryblock.map.entry] [%tryblock.map.entry { i32 3, i32 5, i32 6, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test5@@YAHXZ$2", i32 0, i32 0) }, %tryblock.map.entry { i32 8, i32 10, i32 11, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test5@@YAHXZ$5", i32 0, i32 0) }, %tryblock.map.entry { i32 1, i32 11, i32 12, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test5@@YAHXZ$8", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test5@@YAHXZ" = internal constant [13 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$9") }, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$10") }, %unwind.map.entry { i32 2, i8* null }, %unwind.map.entry { i32 3, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$11") }, %unwind.map.entry { i32 4, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$12") }, %unwind.map.entry { i32 2, i8* null }, %unwind.map.entry { i32 2, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$13") }, %unwind.map.entry { i32 7, i8* null }, %unwind.map.entry { i32 8, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$14") }, %unwind.map.entry { i32 9, i8* blockaddress(@"\01?test5@@YAHXZ", %"\01__unwindfunclet$test5@@YAHXZ$15") }, %unwind.map.entry { i32 7, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

//////////////////////////////////////////////Test 6//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test6@@YAHXZ$3" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUC@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__catch$test6@@YAHXZ$1") }]

// CHECK: @"\01__catchsym$test6@@YAHXZ$5" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__catch$test6@@YAHXZ$0") }]

// CHECK: @"\01__catchsym$test6@@YAHXZ$8" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUB@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__catch$test6@@YAHXZ$6") }]

// CHECK: @"\01__catchsym$test6@@YAHXZ$11" = internal constant [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUC@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__catch$test6@@YAHXZ$9") }]

// CHECK: @"\01__tryblocktable$test6@@YAHXZ" = internal constant [4 x %tryblock.map.entry] [%tryblock.map.entry { i32 9, i32 9, i32 10, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test6@@YAHXZ$3", i32 0, i32 0) }, %tryblock.map.entry { i32 4, i32 6, i32 7, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test6@@YAHXZ$5", i32 0, i32 0) }, %tryblock.map.entry { i32 13, i32 15, i32 16, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test6@@YAHXZ$8", i32 0, i32 0) }, %tryblock.map.entry { i32 2, i32 16, i32 17, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test6@@YAHXZ$11", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test6@@YAHXZ" = internal constant [19 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$12") }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$13") }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 2, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$14") }, %unwind.map.entry { i32 3, i8* null }, %unwind.map.entry { i32 4, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$15") }, %unwind.map.entry { i32 5, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$16") }, %unwind.map.entry { i32 3, i8* null }, %unwind.map.entry { i32 3, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$17") }, %unwind.map.entry { i32 8, i8* null }, %unwind.map.entry { i32 8, i8* null }, %unwind.map.entry { i32 8, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$18") }, %unwind.map.entry { i32 11, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$19") }, %unwind.map.entry { i32 12, i8* null }, %unwind.map.entry { i32 13, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$20") }, %unwind.map.entry { i32 14, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$21") }, %unwind.map.entry { i32 12, i8* null }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* blockaddress(@"\01?test6@@YAHXZ", %"\01__unwindfunclet$test6@@YAHXZ$22") }]

//////////////////////////////////////////////////////////////////////////////////////////////////

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
