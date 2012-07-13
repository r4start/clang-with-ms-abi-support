// RUN: %clang_cc1 -fms-extensions -fblocks -emit-llvm %s -o - -cxx-abi microsoft -triple=i386-pc-win32 | FileCheck %s

namespace test1 { namespace test2 { namespace test3 { namespace test4 { namespace test5 {
namespace test6 { namespace test7 { namespace test8 { namespace test9 { namespace test10 {
namespace test11 {
  class IA
  {
  public:
    IA(){}
    virtual void ia() = 0;
  };

  class ICh : public virtual IA
  {
  public:
    ICh(){}
    virtual void ia(){}
    virtual void iCh(){}
  };
  struct f {
    virtual int asd() {return -90;}
  };

  struct s : public virtual f{
    s(){}
    int r;
    virtual int asd() {return -9;}
  };

  struct sd : virtual s, virtual ICh {
    sd(){}
    ~sd(){}
    virtual int asd() {return -1;}
  };
  
  int test1() {
    sd ft;
    return 0;
  }
  
}}}}}}}}}}}

// CHECK: @"\01??_7sd@test11@test10@test9@test8@test7@test6@test5@test4@test3@test2@test1@@6B@"
// CHECK: @"\01??_7sd@test11@test10@test9@test8@test7@test6@test5@test4@test3@test2@test1@@6BIA@123456789test2@test1@@@"
// CHECK: @"\01??_7sd@test11@test10@test9@test8@test7@test6@test5@test4@test3@test2@test1@@6BICh@123456789test2@test1@@@"

// CHECK: @"\01??_7ICh@test11@test10@test9@test8@test7@test6@test5@test4@test3@test2@test1@@6B0123456789test2@test1@@@"
// CHECK: @"\01??_7ICh@test11@test10@test9@test8@test7@test6@test5@test4@test3@test2@test1@@6BIA@123456789test2@test1@@@"

// CHECK: @"\01??_7IA@test11@test10@test9@test8@test7@test6@test5@test4@test3@test2@test1@@6B@"

// CHECK: @"\01??_7s@test11@test10@test9@test8@test7@test6@test5@test4@test3@test2@test1@@6B@"

// CHECK: @"\01??_7f@test11@test10@test9@test8@test7@test6@test5@test4@test3@test2@test1@@6B@"
