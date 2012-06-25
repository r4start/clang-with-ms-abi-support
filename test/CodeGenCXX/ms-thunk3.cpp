// RUN: %clang_cc1 -fms-extensions -fblocks -emit-llvm %s -o - -cxx-abi microsoft -triple=i386-pc-win32 | FileCheck %s
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
  virtual int asd() {return -1;}
};
int main() {
  sd ft;
  IA *y = &ft;
  y->ia();
  return 0;
}

/////////  VFTables in sd start

// FIXME: @"\01??_7sd@@6B@@@"
// FIXME: @"\01??_R4sd@@6B@@@"
// FIXME: @"\01?asd@sd@@$4PPPPPPPM@A@AEHXZ"
// FIXME: @"\01?ia@ICh@@$R4BI@BA@PPPPPPPM@M@AEXXZ"

// CHECK: @"\01??_7sd@@6B@" = linkonce_odr unnamed_addr constant [2 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4sd@@6B@" to i8*), i8* bitcast (i32 (%struct.sd*)* @"\01?asd@sd@@$4-VTORDISP-PPPPPPPM@UAEHXZ" to i8*)]

// CHECK: @"\01??_7sd@@6BIA@@@" = linkonce_odr unnamed_addr constant [2 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4sd@@6BIA@@@" to i8*), i8* bitcast (void (%class.ICh*)* @"\01?ia@ICh@@$R4-VTORDISP-BI@PPPPPPPM@UAEXXZ" to i8*)]

// CHECK: @"\01??_7sd@@6BICh@@@" = linkonce_odr unnamed_addr constant [2 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4sd@@6BICh@@@" to i8*), i8* bitcast (void (%class.ICh*)* @"\01?iCh@ICh@@UAEXXZ" to i8*)]

/////////  VFTables in sd end

/////////  VFTables in ICh start

// FIXME:@"\01?ia@ICh@@$4PPPPPPPM@A@AEXXZ"

// CHECK: @"\01??_7ICh@@6B0@@" = linkonce_odr unnamed_addr constant [2 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4ICh@@6B0@@" to i8*), i8* bitcast (void (%class.ICh*)* @"\01?iCh@ICh@@UAEXXZ" to i8*)]

// CHECK: @"\01??_7ICh@@6BIA@@@" = linkonce_odr unnamed_addr constant [2 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4ICh@@6BIA@@@" to i8*), i8* bitcast (void (%class.ICh*)* @"\01?ia@ICh@@$4-VTORDISP-PPPPPPPM@UAEXXZ" to i8*)]

/////////  VFTables in ICh end

/////////  VFTables in s start

// FIXME: @"\01??_7s@@6B@@@"
// FIXME: @"\01??_R4s@@6B@@@"
// FIXME: @"\01?asd@s@@$4PPPPPPPM@A@AEHXZ"

// CHECK: @"\01??_7s@@6B@" = linkonce_odr unnamed_addr constant [2 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4s@@6B@" to i8*), i8* bitcast (i32 (%struct.s*)* @"\01?asd@s@@$4-VTORDISP-PPPPPPPM@UAEHXZ" to i8*)]

/////////  VFTables in s end


// CHECK:      define linkonce_odr i32 @"\01?asd@sd@@$4-VTORDISP-PPPPPPPM@UAEHXZ"(%struct.sd* %this) {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %this.addr = alloca %struct.sd*, align 4
// CHECK-NEXT:   store %struct.sd* %this, %struct.sd** %this.addr, align 4
// CHECK-NEXT:   %this1 = load %struct.sd** %this.addr
// CHECK-NEXT:   %this.i8.ptr = bitcast %struct.sd* %this1 to i8*
// CHECK-NEXT:   %vtordisp.ptr = getelementptr i8* %this.i8.ptr, i32 -4
// CHECK-NEXT:   %vtordisp = bitcast i8* %vtordisp.ptr to i32*
// CHECK-NEXT:   %vtordisp.offset = load i32* %vtordisp
// CHECK-NEXT:   %vtordisp.this = getelementptr inbounds i8* %this.i8.ptr, i32 %vtordisp.offset
// CHECK-NEXT:   %final.this = bitcast i8* %vtordisp.this to %struct.sd*
// CHECK-NEXT:   %call = call i32 @"\01?asd@sd@@UAEHXZ"(%struct.sd* %final.this)
// CHECK-NEXT:   ret i32 %call
// CHECK-NEXT: }

// CHECK:      define linkonce_odr void @"\01?ia@ICh@@$R4-VTORDISP-BI@PPPPPPPM@UAEXXZ"(%class.ICh* %this) {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %this.addr = alloca %class.ICh*, align 4
// CHECK-NEXT:   store %class.ICh* %this, %class.ICh** %this.addr, align 4
// CHECK-NEXT:   %this1 = load %class.ICh** %this.addr
// CHECK-NEXT:   %this.i8.ptr = bitcast %class.ICh* %this1 to i8*
// CHECK-NEXT:   %vtordisp.ptr = getelementptr i8* %this.i8.ptr, i32 -4
// CHECK-NEXT:   %vtordisp = bitcast i8* %vtordisp.ptr to i32*
// CHECK-NEXT:   %vtordisp.offset = load i32* %vtordisp
// CHECK-NEXT:   %vtordisp.this = getelementptr inbounds i8* %this.i8.ptr, i32 %vtordisp.offset
// CHECK-NEXT:   %vbptr.ptr = getelementptr i8* %vtordisp.this, i32 0
// CHECK-NEXT:   %vbptr = bitcast i8* %vbptr.ptr to i32*
// CHECK-NEXT:   %vbtable.offset = getelementptr i32* %vbptr, i32 4
// CHECK-NEXT:   %vbase.offset = load i32* %vbtable.offset
// CHECK-NEXT:   %this.plus.vb.offset = getelementptr i8* %vtordisp.this, i32 %vbase.offset
// CHECK-NEXT:   %adjusted.this = getelementptr i8* %this.plus.vb.offset, i32 -12
// CHECK-NEXT:   %final.this = bitcast i8* %adjusted.this to %class.ICh*
// CHECK-NEXT:   call void @"\01?ia@ICh@@UAEXXZ"(%class.ICh* %final.this)
// CHECK-NEXT:   ret void
// CHECK-NEXT: }

// CHECK:      define available_externally void @"\01?ia@ICh@@$4-VTORDISP-PPPPPPPM@UAEXXZ"(%class.ICh* %this) {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %this.addr = alloca %class.ICh*, align 4
// CHECK-NEXT:   store %class.ICh* %this, %class.ICh** %this.addr, align 4
// CHECK-NEXT:   %this1 = load %class.ICh** %this.addr
// CHECK-NEXT:   %this.i8.ptr = bitcast %class.ICh* %this1 to i8*
// CHECK-NEXT:   %vtordisp.ptr = getelementptr i8* %this.i8.ptr, i32 -4
// CHECK-NEXT:   %vtordisp = bitcast i8* %vtordisp.ptr to i32*
// CHECK-NEXT:   %vtordisp.offset = load i32* %vtordisp
// CHECK-NEXT:   %vtordisp.this = getelementptr inbounds i8* %this.i8.ptr, i32 %vtordisp.offset
// CHECK-NEXT:   %final.this = bitcast i8* %vtordisp.this to %class.ICh*
// CHECK-NEXT:   call void @"\01?ia@ICh@@UAEXXZ"(%class.ICh* %final.this)
// CHECK-NEXT:   ret void
// CHECK-NEXT: }

// CHECK:      define linkonce_odr i32 @"\01?asd@s@@$4-VTORDISP-PPPPPPPM@UAEHXZ"(%struct.s* %this) {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %this.addr = alloca %struct.s*, align 4
// CHECK-NEXT:   store %struct.s* %this, %struct.s** %this.addr, align 4
// CHECK-NEXT:   %this1 = load %struct.s** %this.addr
// CHECK-NEXT:   %this.i8.ptr = bitcast %struct.s* %this1 to i8*
// CHECK-NEXT:   %vtordisp.ptr = getelementptr i8* %this.i8.ptr, i32 -4
// CHECK-NEXT:   %vtordisp = bitcast i8* %vtordisp.ptr to i32*
// CHECK-NEXT:   %vtordisp.offset = load i32* %vtordisp
// CHECK-NEXT:   %vtordisp.this = getelementptr inbounds i8* %this.i8.ptr, i32 %vtordisp.offset
// CHECK-NEXT:   %final.this = bitcast i8* %vtordisp.this to %struct.s*
// CHECK-NEXT:   %call = call i32 @"\01?asd@s@@UAEHXZ"(%struct.s* %final.this)
// CHECK-NEXT:   ret i32 %call
// CHECK-NEXT: }