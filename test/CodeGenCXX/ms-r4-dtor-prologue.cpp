// RUN: %clang_cc1 -fms-extensions -fblocks -emit-llvm %s -o - -cxx-abi microsoft -triple=i386-pc-win32 | FileCheck %s

// In this test we check destructor prologue for class with virtual bases.
// Destructor prologue reinitialize all vf-tables of the class.

namespace test1 {
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
}

// CHECK:      define linkonce_odr x86_thiscallcc void @"\01??1sd@test1@@QAE@XZ"(%"struct.test1::sd"* %this) unnamed_addr nounwind align 2 {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %this.addr = alloca %"struct.test1::sd"*, align 4
// CHECK-NEXT:   store %"struct.test1::sd"* %this, %"struct.test1::sd"** %this.addr, align 4
// CHECK-NEXT:   %this1 = load %"struct.test1::sd"** %this.addr
// CHECK-NEXT:   %0 = bitcast %"struct.test1::sd"* %this1 to i8*
// CHECK-NEXT:   %vfptr.field = getelementptr inbounds i8* %0, i64 8
// CHECK-NEXT:   %vfptr = bitcast i8* %vfptr.field to i8***
// CHECK-NEXT:   store i8** getelementptr inbounds ([2 x i8*]* @"\01??_7sd@test1@@6B@", i64 0, i64 1), i8*** %vfptr
// CHECK-NEXT:   %1 = bitcast %"struct.test1::sd"* %this1 to i8*
// CHECK-NEXT:   %vfptr.field2 = getelementptr inbounds i8* %1, i64 24
// CHECK-NEXT:   %vfptr3 = bitcast i8* %vfptr.field2 to i8***
// CHECK-NEXT:   store i8** getelementptr inbounds ([2 x i8*]* @"\01??_7sd@test1@@6BIA@1@@", i64 0, i64 1), i8*** %vfptr3
// CHECK-NEXT:   %2 = bitcast %"struct.test1::sd"* %this1 to i8*
// CHECK-NEXT:   %vfptr.field4 = getelementptr inbounds i8* %2, i64 28
// CHECK-NEXT:   %vfptr5 = bitcast i8* %vfptr.field4 to i8***
// CHECK-NEXT:  store i8** getelementptr inbounds ([2 x i8*]* @"\01??_7sd@test1@@6BICh@1@@", i64 0, i64 1), i8*** %vfptr5
// CHECK-NEXT:   %vtordisp.this = bitcast %"struct.test1::sd"* %this1 to i8*
// CHECK-NEXT:   %vbtable.ptr = getelementptr inbounds i8* %vtordisp.this, i32 0
// CHECK-NEXT:   %vbtable.i32.ptr = bitcast i8* %vbtable.ptr to i32*
// CHECK-NEXT:   %offset.from.table = getelementptr i32* %vbtable.i32.ptr, i32 1
// CHECK-NEXT:   %vb.offset = load i32* %offset.from.table
// CHECK-NEXT:   %vtordisp.val = sub i32 %vb.offset, 8
// CHECK-NEXT:   %3 = getelementptr i8* %vtordisp.this, i32 4
// CHECK-NEXT:   %4 = bitcast i8* %3 to i32*
// CHECK-NEXT:   store i32 %vtordisp.val, i32* %4
// CHECK-NEXT:   %vtordisp.this6 = bitcast %"struct.test1::sd"* %this1 to i8*
// CHECK-NEXT:   %vbtable.ptr7 = getelementptr inbounds i8* %vtordisp.this6, i32 0
// CHECK-NEXT:   %vbtable.i32.ptr8 = bitcast i8* %vbtable.ptr7 to i32*
// CHECK-NEXT:   %offset.from.table9 = getelementptr i32* %vbtable.i32.ptr8, i32 3
// CHECK-NEXT:   %vb.offset10 = load i32* %offset.from.table9
// CHECK-NEXT:   %vtordisp.val11 = sub i32 %vb.offset10, 24
// CHECK-NEXT:   %5 = getelementptr i8* %vtordisp.this6, i32 20
// CHECK-NEXT:   %6 = bitcast i8* %5 to i32*
// CHECK-NEXT:   store i32 %vtordisp.val11, i32* %6
// CHECK-NEXT:   ret void
// CHECK-NEXT: }