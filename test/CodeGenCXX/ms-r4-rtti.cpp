// RUN: %clang_cc1 -fms-extensions -fblocks -emit-llvm %s -o - -cxx-abi microsoft -triple=i386-pc-win32 | FileCheck %s

// r4start
// Test for properly building RTTI information.
// Vf-table check is necessary because vf-table contains pointer to RTTI info.
#pragma pack(push, 8)
class first {
public:
  virtual void b() {}
  double b_field;
protected:
private:
};

class second : first {
public:
  virtual void C_f(){}
  int y;
};

class third : protected second
{
public:
  int a_field;
  virtual void a(){}
protected:
private:
};
#pragma pack(pop)

int main() {
  third k;
  k.a();
  return 0;
}

//CHECK: @"\01??_7third@@6B@" = linkonce_odr unnamed_addr constant [4 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4third@@6B@" to i8*), i8* bitcast (void (%class.first*)* @"\01?b@first@@UAEXXZ" to i8*), i8* bitcast (void (%class.second*)* @"\01?C_f@second@@UAEXXZ" to i8*), i8* bitcast (void (%class.third*)* @"\01?a@third@@UAEXXZ" to i8*)]
//CHECK: @"\01??_7type_info@@6B@" = external global i8*
//CHECK: @"\01??_R0?AVthird@@@8" = constant { i8**, i32, [12 x i8] } { i8** @"\01??_7type_info@@6B@", i32 0, [12 x i8] c".?AVthird@@\00" }
//CHECK: @"\01??_R1A@?0A@EA@third@@8" = constant { i8*, i32, i32, i32, i32, i32 } { i8* bitcast ({ i8**, i32, [12 x i8] }* @"\01??_R0?AVthird@@@8" to i8*), i32 2, i32 0, i32 -1, i32 0, i32 64 }
//CHECK: @"\01??_R0?AVsecond@@@8" = constant { i8**, i32, [13 x i8] } { i8** @"\01??_7type_info@@6B@", i32 0, [13 x i8] c".?AVsecond@@\00" }
//CHECK: @"\01??_R1A@?0A@EN@second@@8" = constant { i8*, i32, i32, i32, i32, i32 } { i8* bitcast ({ i8**, i32, [13 x i8] }* @"\01??_R0?AVsecond@@@8" to i8*), i32 1, i32 0, i32 -1, i32 0, i32 77 }
//CHECK: @"\01??_R0?AVfirst@@@8" = constant { i8**, i32, [12 x i8] } { i8** @"\01??_7type_info@@6B@", i32 0, [12 x i8] c".?AVfirst@@\00" }
//CHECK: @"\01??_R1A@?0A@EN@first@@8" = constant { i8*, i32, i32, i32, i32, i32 } { i8* bitcast ({ i8**, i32, [12 x i8] }* @"\01??_R0?AVfirst@@@8" to i8*), i32 0, i32 0, i32 -1, i32 0, i32 77 }
//CHECK: @"\01??_R2third@@8" = constant [3 x { i8*, i32, i32, i32, i32, i32 }*] [{ i8*, i32, i32, i32, i32, i32 }* @"\01??_R1A@?0A@EA@third@@8", { i8*, i32, i32, i32, i32, i32 }* @"\01??_R1A@?0A@EN@second@@8", { i8*, i32, i32, i32, i32, i32 }* @"\01??_R1A@?0A@EN@first@@8"]
//CHECK: @"\01??_R3third@@8" = constant { i32, i32, i32, i8* } { i32 0, i32 0, i32 3, i8* bitcast ([3 x { i8*, i32, i32, i32, i32, i32 }*]* @"\01??_R2third@@8" to i8*) }
//CHECK: @"\01??_R4third@@6B@" = unnamed_addr constant { i32, i32, i32, i8*, i8* } { i32 0, i32 0, i32 0, i8* bitcast ({ i8**, i32, [12 x i8] }* @"\01??_R0?AVthird@@@8" to i8*), i8* bitcast ({ i32, i32, i32, i8* }* @"\01??_R3third@@8" to i8*) }
//CHECK: @"\01??_7second@@6B@" = linkonce_odr unnamed_addr constant [3 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4second@@6B@" to i8*), i8* bitcast (void (%class.first*)* @"\01?b@first@@UAEXXZ" to i8*), i8* bitcast (void (%class.second*)* @"\01?C_f@second@@UAEXXZ" to i8*)]
//CHECK: @"\01??_R1A@?0A@EA@second@@8" = constant { i8*, i32, i32, i32, i32, i32 } { i8* bitcast ({ i8**, i32, [13 x i8] }* @"\01??_R0?AVsecond@@@8" to i8*), i32 1, i32 0, i32 -1, i32 0, i32 64 }
//CHECK: @"\01??_R2second@@8" = constant [2 x { i8*, i32, i32, i32, i32, i32 }*] [{ i8*, i32, i32, i32, i32, i32 }* @"\01??_R1A@?0A@EA@second@@8", { i8*, i32, i32, i32, i32, i32 }* @"\01??_R1A@?0A@EN@first@@8"]
//CHECK: @"\01??_R3second@@8" = constant { i32, i32, i32, i8* } { i32 0, i32 0, i32 2, i8* bitcast ([2 x { i8*, i32, i32, i32, i32, i32 }*]* @"\01??_R2second@@8" to i8*) }
//CHECK: @"\01??_R4second@@6B@" = unnamed_addr constant { i32, i32, i32, i8*, i8* } { i32 0, i32 0, i32 0, i8* bitcast ({ i8**, i32, [13 x i8] }* @"\01??_R0?AVsecond@@@8" to i8*), i8* bitcast ({ i32, i32, i32, i8* }* @"\01??_R3second@@8" to i8*) }
//CHECK: @"\01??_7first@@6B@" = linkonce_odr unnamed_addr constant [2 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4first@@6B@" to i8*), i8* bitcast (void (%class.first*)* @"\01?b@first@@UAEXXZ" to i8*)]
//CHECK: @"\01??_R1A@?0A@EA@first@@8" = constant { i8*, i32, i32, i32, i32, i32 } { i8* bitcast ({ i8**, i32, [12 x i8] }* @"\01??_R0?AVfirst@@@8" to i8*), i32 0, i32 0, i32 -1, i32 0, i32 64 }
//CHECK: @"\01??_R2first@@8" = constant [1 x { i8*, i32, i32, i32, i32, i32 }*] [{ i8*, i32, i32, i32, i32, i32 }* @"\01??_R1A@?0A@EA@first@@8"]
//CHECK: @"\01??_R3first@@8" = constant { i32, i32, i32, i8* } { i32 0, i32 0, i32 1, i8* bitcast ([1 x { i8*, i32, i32, i32, i32, i32 }*]* @"\01??_R2first@@8" to i8*) }
//CHECK: @"\01??_R4first@@6B@" = unnamed_addr constant { i32, i32, i32, i8*, i8* } { i32 0, i32 0, i32 0, i8* bitcast ({ i8**, i32, [12 x i8] }* @"\01??_R0?AVfirst@@@8" to i8*), i8* bitcast ({ i32, i32, i32, i8* }* @"\01??_R3first@@8" to i8*) }