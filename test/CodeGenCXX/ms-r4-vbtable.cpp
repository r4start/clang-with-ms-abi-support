// RUN: %clang_cc1 -fms-extensions -fblocks -emit-llvm %s -o - -cxx-abi microsoft -triple=i386-pc-win32 | FileCheck %s

// r4start
// Test for vb-table layout.

// FIXME: Fix vb-table name mangling with namespaces.
// CHECK: @"\01??_8s@test4@@7B@" = unnamed_addr constant [3 x i32] [i32 0, i32 8, i32 16]
// CHECK: @"\01??_8s@test3@@7B@" = unnamed_addr constant [2 x i32] [i32 0, i32 8]
// CHECK: @"\01??_8third@test2@@7B@" = unnamed_addr constant [2 x i32] [i32 -8, i32 8]
// CHECK: @"\01??_8third@test1@@7B@" = unnamed_addr constant [2 x i32] [i32 -4, i32 4]

namespace test1 {
class first {
public:
  virtual void a(){}
};

class third: virtual first {
public:
  virtual void b(){}
};

void test() {
  third a;
}
}

namespace test2 {
class first {
public:
  virtual void b(){}
  int b_field;
};

class second : first {
public:
  virtual void C_f(){}
  int y;
};

class fourth
{
public:
  int q;
};

class third : public  fourth,  virtual second {
public:
  int a_field;
  virtual void a(){}
};

void test() {
  third a;
}
}

namespace test3 {
struct f {
  virtual int asd() {return -90;}
};

struct s : public virtual f {
  s(){}
  virtual int asd() {return -9;}
};

void test() { 
  s ss; 
}
}

namespace test4 {

struct f {
  virtual int asd() {return -90;}
};

struct fd {
  virtual int asfd() {return -90;}
};

struct s : public virtual f, public virtual fd {
  s(){}
  virtual int asd() {return -9;}
  virtual int asfd() {return -9;}
};

void test2() { s ss; }

}
