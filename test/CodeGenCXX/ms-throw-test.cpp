// RUN: %clang_cc1  -emit-llvm %s -o - -cxx-abi microsoft -fcxx-exceptions -fexceptions -triple=i686-pc-win32-microsoft | FileCheck %s
// r4start

struct A {
 A() { }
 ~A() {
 }
};

struct B : A {
 B() { }
 ~B() { }
};

// CHECK: @"\01__CT??_R0H@84" = constant %catchable.type { i32 1, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), %pmd.type { i32 0, i32 -1, i32 0 }, i32 4, void ()* null }
// CHECK: @"\01__CTA1H" = constant { i32, [1 x %catchable.type*] } { i32 1, [1 x %catchable.type*] [%catchable.type* @"\01__CT??_R0H@84"] }
// CHECK: @"\01__TI1H" = constant %throw.info.type { i32 0, void ()* null, i32 ()* null, %catchable.array.type* bitcast ({ i32, [1 x %catchable.type*] }* @"\01__CTA1H" to %catchable.array.type*) }

int test1() {
  throw 1;
  return 0;
}

// CHECK: @"\01__CT??_R0?AUA@@@81" = constant %catchable.type { i32 0, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), %pmd.type { i32 0, i32 -1, i32 0 }, i32 1, void ()* null }
// CHECK: @"\01__CTA1?AUA@@" = constant { i32, [1 x %catchable.type*] } { i32 1, [1 x %catchable.type*] [%catchable.type* @"\01__CT??_R0?AUA@@@81"] }
// CHECK: @"\01__TI1?AUA@@" = constant %throw.info.type { i32 0, void ()* bitcast (void (%struct.A*)* @"\01??1A@@QAE@XZ" to void ()*), i32 ()* null, %catchable.array.type* bitcast ({ i32, [1 x %catchable.type*] }* @"\01__CTA1?AUA@@" to %catchable.array.type*) }

int test2() {
  throw A();
  return 0;
}

// CHECK: @"\01??_R0?AUB@@@8" = constant { i8**, i32, [8 x i8] } { i8** @"\01??_7type_info@@6B@", i32 0, [8 x i8] c".?AUB@@\00" }
// CHECK: @"\01__CT??_R0?AUB@@@81" = constant %catchable.type { i32 0, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUB@@@8" to %type.descriptor*), %pmd.type { i32 0, i32 -1, i32 0 }, i32 1, void ()* null }
// CHECK: @"\01__CTA2?AUB@@" = constant { i32, [2 x %catchable.type*] } { i32 2, [2 x %catchable.type*] [%catchable.type* @"\01__CT??_R0?AUB@@@81", %catchable.type* @"\01__CT??_R0?AUA@@@81"] }
// CHECK: @"\01__TI2?AUB@@" = constant %throw.info.type { i32 0, void ()* bitcast (void (%struct.B*)* @"\01??1B@@QAE@XZ" to void ()*), i32 ()* null, %catchable.array.type* bitcast ({ i32, [2 x %catchable.type*] }* @"\01__CTA2?AUB@@" to %catchable.array.type*) }

int test3() {
  throw B();
  return 0;
}
