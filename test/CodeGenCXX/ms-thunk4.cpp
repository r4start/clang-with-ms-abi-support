// RUN: %clang_cc1 -fms-extensions -fblocks -emit-llvm %s -o - -cxx-abi microsoft -triple=i386-pc-win32 | FileCheck %s
#pragma pack(push, 8)
struct null {
  virtual void uiiiii(){}
};
class first {
public:
  virtual void asdf(){}
};
class second : public virtual first, public null {
public :
	virtual void asdf(){}
	virtual void uiiiii(){}
	virtual ~second(){}
	second(){}
};
#pragma pack(pop)

int main() {
	second f;
  return 0;
}

// Насколько я понимаю PPPPPPPM@A@A это все какие-то числа и скорее всего всякие оффсеты, аналогично как и некоторые структуры RTTI "манглятся".
// FIXME: @"\01?asdf@second@@$4PPPPPPPM@A@AEXXZ"

// CHECK: @"\01??_7second@@6Bfirst@@@" = linkonce_odr unnamed_addr constant [2 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4second@@6Bfirst@@@" to i8*), i8* bitcast (void (%class.second*)* @"\01?asdf@second@@$4PPPPPPPM@A@AEXXZ" to i8*)]

// CHECK: @"\01??_7second@@6Bnull@@@" = linkonce_odr unnamed_addr constant [3 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4second@@6Bnull@@@" to i8*), i8* bitcast (void (%class.second*)* @"\01?uiiiii@second@@UAEXXZ" to i8*), i8* bitcast (void (%class.second*)* @"\01??1second@@UAE@XZ" to i8*)]

// CHECK:      define linkonce_odr x86_thiscallcc void @"\01?asdf@second@@$4PPPPPPPM@A@AEXXZ"(%class.second* %this) {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %this.addr = alloca %class.second*, align 4
// CHECK-NEXT:   store %class.second* %this, %class.second** %this.addr, align 4
// CHECK-NEXT:   %this1 = load %class.second** %this.addr
// CHECK-NEXT:   %this.i8.ptr = bitcast %class.second* %this1 to i8*
// CHECK-NEXT:   %vtordisp.ptr = getelementptr i8* %this.i8.ptr, i32 -4
// CHECK-NEXT:   %vtordisp = bitcast i8* %vtordisp.ptr to i32*
// CHECK-NEXT:   %vtordisp.offset = load i32* %vtordisp
// CHECK-NEXT:   %vtordisp.this = getelementptr inbounds i8* %this.i8.ptr, i32 %vtordisp.offset
// CHECK-NEXT:   %final.this = bitcast i8* %vtordisp.this to %class.second*
// CHECK-NEXT:   call x86_thiscallcc void @"\01?asdf@second@@UAEXXZ"(%class.second* %final.this)
// CHECK-NEXT:   ret void
// CHECK-NEXT: }