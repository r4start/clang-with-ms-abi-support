// RUN: %clang_cc1 -fno-rtti -fms-extensions -fblocks -emit-llvm %s -o - -cxx-abi microsoft -triple=i386-pc-win32 | FileCheck %s

// r4start
// Test for vb-table layout.

// FIXME: Fix vb-table name mangling with namespaces.
// CHECK: @"\01??_8s@test4@@7B@" = weak unnamed_addr constant [3 x i32] [i32 0, i32 8, i32 16]
// CHECK: @"\01??_8s@test3@@7B@" = weak unnamed_addr constant [2 x i32] [i32 0, i32 8]
// CHECK: @"\01??_8third@test2@@7B@" = weak unnamed_addr constant [2 x i32] [i32 -8, i32 8]
// CHECK: @"\01??_8third@test1@@7B@" = weak unnamed_addr constant [2 x i32] [i32 -4, i32 4]

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

// CHECK: @"\01??_8DD@test5@@7BvBB@1@@" = weak unnamed_addr constant [3 x i32] [i32 0, i32 12, i32 16]
// CHECK: @"\01??_8DD@test5@@7BB@1@@" = weak unnamed_addr constant [3 x i32] [i32 -8, i32 20, i32 24]
// CHECK: @"\01??_8D@test5@@7BvBB@1@@" = weak unnamed_addr constant [3 x i32] [i32 0, i32 12, i32 16]
// CHECK: @"\01??_8D@test5@@7BB@1@@" = weak unnamed_addr constant [3 x i32] [i32 -8, i32 20, i32 24]
// CHECK: @"\01??_8vBB@test5@@7B@" = weak unnamed_addr constant [3 x i32] [i32 0, i32 8, i32 12]
// CHECK: @"\01??_8B@test5@@7B@" = weak unnamed_addr constant [2 x i32] [i32 -8, i32 8]
namespace test5 {

struct A
{
  int a;
  void afunc()
  {}
};

struct AA
{
  int aa;
  void aafunc()
  {}
};

struct AAA
{
  int aaa;
  void aaafunc()
  {}
};

struct B : public AAA, virtual public A
{
  virtual void a(){}
  int b;
};

struct BBBB
{
  int bbbb;
  virtual void a(){}
  BBBB()
    : bbbb(0xDEADBEEF)
  {}
};

struct vBB : virtual public A, virtual public AA
{
  int vbb;
};

struct D : public BBBB, public B, public vBB
{
  int d;
  void dfunc()
  {}
};
struct DD : public BBBB, public D
{
  void ddfunc()
  {}
};


int test()
{
  DD d;
  return 0;
}

}
