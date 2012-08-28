// RUN: %clang_cc1 -emit-llvm %s -o - -cxx-abi microsoft -triple=i386-pc-win32 | FileCheck %s

struct S {
  S() {}
  ~S() {}
} s;

// CHECK: define internal void [[INIT_s:@.*global_var.*]] nounwind
// CHECK: %call = call %struct.S* @"\01??0S@@QAE@XZ"(%struct.S* @"\01?s@@3US@@A")
// CHECK: %0 = call i32 @atexit(void ()* @"__dtor_\01?s@@3US@@A") nounwind
// CHECK: ret void

// CHECK: define internal void @"__dtor_\01?s@@3US@@A"() nounwind {
// CHECK: call void @"\01??1S@@QAE@XZ"
// CHECK: ret void

// Force WeakODRLinkage by using templates
class A {
 public:
  A() {}
  ~A() {}
};

template<typename T>
class B {
 public:
  static A foo;
};

template<typename T> A B<T>::foo;

void force_usage() {
  (void)B<int>::foo;  // (void) - force usage
}

// CHECK: define internal void [[INIT_foo:@.*global_var.*]] nounwind
// CHECK: call %class.A* @"\01??0A@@QAE@XZ"
// CHECK: call i32 @atexit(void ()* [[FOO_DTOR:@"__dtor_.*foo@.*]])
// CHECK: ret void

// CHECK: define linkonce_odr %class.A* @"\01??0A@@QAE@XZ"

// CHECK: define linkonce_odr void @"\01??1A@@QAE@XZ"

// CHECK: define internal void [[FOO_DTOR]]
// CHECK: call void @"\01??1A@@QAE@XZ"{{.*}}foo
// CHECK: ret void

// CHECK: define internal void @_GLOBAL__I_a() nounwind {
// CHECK: call void [[INIT_s]]
// CHECK: call void [[INIT_foo]]
// CHECK: ret void
