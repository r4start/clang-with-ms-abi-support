// RUN: %clang_cc1  -emit-llvm %s -o - -cxx-abi microsoft -triple=i686-pc-win32 | FileCheck %s
// r4start

#define APIENTRY __stdcall
#define BOOL int
#define TRUE 1
typedef unsigned long DWORD;
#define LPVOID void*

#define DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name
DECLARE_HANDLE(HINSTANCE);
typedef HINSTANCE HMODULE;

// CHECK: define x86_stdcallcc i32 @DllMain(%struct.HINSTANCE__* %hModule, i32 %ul_reason_for_call, i8* %lpReserved)

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
           )
{
  switch (ul_reason_for_call)
  {
    default:
      break;
  }
  return TRUE;
}