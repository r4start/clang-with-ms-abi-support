// RUN: %clang_cc1 -fms-extensions -fblocks -emit-llvm %s -o - -cxx-abi microsoft -triple=i386-pc-win32 | FileCheck %s
#pragma pack(push, 8)
class first{
  int s;
public:
  virtual void asdf() {}
	virtual void g(){}
};
class second : public virtual first {
public :
  int q;
	virtual void asdf() { q = 90; }
	virtual void g(){ q = 12; }
};
class third : public virtual second {
public:
  int qq;
	virtual void g(){}
};
#pragma pack(pop)

int main() {
  third s;
  first *fg = (first *)&s;
  fg->asdf();
  return 0;
}

// FIXME: @"\01??_7third@@6B@@@"
// FIXME: @"\01??_R4third@@6B@@@"
// FIXME: @"\01?asdf@second@@WPPPPPPPA@AEXXZ"

// CHECK: @"\01??_7third@@6B@" = linkonce_odr unnamed_addr constant [3 x i8*] [i8* bitcast ({ i32, i32, i32, i8*, i8* }* @"\01??_R4third@@6B@" to i8*), i8* bitcast (void (%class.second*)* @"\01?asdf@second@@WPPPPPPPA@UAEXXZ" to i8*), i8* bitcast (void (%class.third*)* @"\01?g@third@@UAEXXZ" to i8*)]

// CHECK:      define linkonce_odr void @"\01?asdf@second@@WPPPPPPPA@UAEXXZ"(%class.second* %this) {
// CHECK-NEXT: entry:
// CHECK-NEXT:   %this.addr = alloca %class.second*, align 4
// CHECK-NEXT:   store %class.second* %this, %class.second** %this.addr, align 4
// CHECK-NEXT:   %this1 = load %class.second** %this.addr
// CHECK-NEXT:   %this.i8.ptr = bitcast %class.second* %this1 to i8*
// CHECK-NEXT:   %adjusted.this = getelementptr i8* %this.i8.ptr, i32 16
// CHECK-NEXT:   %final.this = bitcast i8* %adjusted.this to %class.second*
// CHECK-NEXT:   call void @"\01?asdf@second@@UAEXXZ"(%class.second* %final.this)
// CHECK-NEXT:   ret void
// CHECK-NEXT: }