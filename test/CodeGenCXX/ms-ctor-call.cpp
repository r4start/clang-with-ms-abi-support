// RUN: %clang_cc1 -fms-extensions -fblocks -emit-llvm %s -o - -cxx-abi microsoft -triple=i386-pc-win32 | FileCheck %s

namespace test {

class A {
  virtual void Aaaa() {}
};

class B : public virtual A {
  virtual void bbb(){}
};

class C : public virtual B {
public:
  C(){}
};

void test1() {
  A a;
  B b;
  C c;
}

}

// CHECK:      %call = call x86_thiscallcc %"class.test::A"* @"\01??0A@test@@QAE@XZ"(%"class.test::A"* %a) nounwind
// CHECK-NEXT: %call1 = call x86_thiscallcc %"class.test::B"* @"\01??0B@test@@QAE@XZ"(%"class.test::B"* %b, i32 zeroext 1) nounwind
// CHECK-NEXT: %call2 = call x86_thiscallcc %"class.test::C"* @"\01??0C@test@@QAE@XZ"(%"class.test::C"* %c, i32 zeroext 1)

// CHECK:      %call4 = call x86_thiscallcc %"class.test::B"* @"\01??0B@test@@QAE@XZ"(%"class.test::B"* %7, i32 zeroext 0) nounwind
