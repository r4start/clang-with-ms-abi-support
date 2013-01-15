// RUN: %clang_cc1  -emit-llvm %s -o - -cxx-abi microsoft -fcxx-exceptions -fexceptions -triple=i686-pc-win32-microsoft | FileCheck %s
// r4start

//////////////////////////////////////////////////////////////////////////////////////////////////

// CHECK: @"\01__CT??_R0H@84" = weak global %catchable.type { i32 1, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), %pmd.type { i32 0, i32 -1, i32 0 }, i32 4, void ()* null }

// CHECK: @"\01__CTA1H" = weak global { i32, [1 x %catchable.type*] } { i32 1, [1 x %catchable.type*] [%catchable.type* @"\01__CT??_R0H@84"] }

// CHECK: @"\01__TI1H" = weak global %throw.info.type { i32 0, void ()* null, i32 ()* null, %catchable.array.type* bitcast ({ i32, [1 x %catchable.type*] }* @"\01__CTA1H" to %catchable.array.type*) }

// CHECK: @"\01??_R0?AUA@@@8" = weak constant { i8**, i32, [8 x i8] } { i8** @"\01??_7type_info@@6B@", i32 0, [8 x i8] c".?AUA@@\00" }

// CHECK: @"\01__CT??_R0?AUA@@@81" = weak global %catchable.type { i32 0, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), %pmd.type { i32 0, i32 -1, i32 0 }, i32 1, void ()* null }

// CHECK: @"\01__CTA1?AUA@@" = weak global { i32, [1 x %catchable.type*] } { i32 1, [1 x %catchable.type*] [%catchable.type* @"\01__CT??_R0?AUA@@@81"] }

// CHECK: @"\01__TI1?AUA@@" = weak global %throw.info.type { i32 0, void ()* bitcast (void (%struct.A*)* @"\01??1A@@QAE@XZ" to void ()*), i32 ()* null, %catchable.array.type* bitcast ({ i32, [1 x %catchable.type*] }* @"\01__CTA1?AUA@@" to %catchable.array.type*) }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 1//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test1@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test1@@YAHXZ", %catch) }]

// CHECK: @"\01__tryblocktable$test1@@YAHXZ" = weak global [1 x %tryblock.map.entry] [%tryblock.map.entry { i32 0, i32 2, i32 3, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test1@@YAHXZ$0", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test1@@YAHXZ" = weak global [4 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test1@@YAHXZ", %ehcleanup) }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test1@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 4, %unwind.map.entry* getelementptr inbounds ([4 x %unwind.map.entry]* @"\01__unwindtable$test1@@YAHXZ", i32 0, i32 0), i32 1, %tryblock.map.entry* getelementptr inbounds ([1 x %tryblock.map.entry]* @"\01__tryblocktable$test1@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 2//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test2@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUB@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test2@@YAHXZ", %catch) }]

// CHECK: @"\01__catchsym$test2@@YAHXZ$1" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test2@@YAHXZ", %catch5) }]

// CHECK: @"\01__tryblocktable$test2@@YAHXZ" = weak global [2 x %tryblock.map.entry] [%tryblock.map.entry { i32 1, i32 1, i32 2, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test2@@YAHXZ$0", i32 0, i32 0) }, %tryblock.map.entry { i32 0, i32 2, i32 3, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test2@@YAHXZ$1", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test2@@YAHXZ" = weak global [4 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry zeroinitializer, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test2@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 4, %unwind.map.entry* getelementptr inbounds ([4 x %unwind.map.entry]* @"\01__unwindtable$test2@@YAHXZ", i32 0, i32 0), i32 2, %tryblock.map.entry* getelementptr inbounds ([2 x %tryblock.map.entry]* @"\01__tryblocktable$test2@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 3//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test3@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test3@@YAHXZ", %catch) }]

// CHECK: @"\01__catchsym$test3@@YAHXZ$1" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test3@@YAHXZ", %catch5) }]

// CHECK: @"\01__tryblocktable$test3@@YAHXZ" = weak global [2 x %tryblock.map.entry] [%tryblock.map.entry { i32 0, i32 0, i32 1, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test3@@YAHXZ$0", i32 0, i32 0) }, %tryblock.map.entry { i32 2, i32 2, i32 3, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test3@@YAHXZ$1", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test3@@YAHXZ" = weak global [4 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test3@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 4, %unwind.map.entry* getelementptr inbounds ([4 x %unwind.map.entry]* @"\01__unwindtable$test3@@YAHXZ", i32 0, i32 0), i32 2, %tryblock.map.entry* getelementptr inbounds ([2 x %tryblock.map.entry]* @"\01__tryblocktable$test3@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 4//////////////////////////////////////////////


// CHECK: @"\01__catchsym$test4@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test4@@YAHXZ", %catch) }]

// CHECK: @"\01__catchsym$test4@@YAHXZ$1" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test4@@YAHXZ", %catch6) }]

// CHECK: @"\01__catchsym$test4@@YAHXZ$2" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUC@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test4@@YAHXZ", %catch9) }]

// CHECK: @"\01__tryblocktable$test4@@YAHXZ" = weak global [3 x %tryblock.map.entry] [%tryblock.map.entry { i32 1, i32 1, i32 2, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test4@@YAHXZ$0", i32 0, i32 0) }, %tryblock.map.entry { i32 3, i32 3, i32 4, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test4@@YAHXZ$1", i32 0, i32 0) }, %tryblock.map.entry { i32 0, i32 4, i32 5, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test4@@YAHXZ$2", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test4@@YAHXZ" = weak global [6 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry zeroinitializer, %unwind.map.entry zeroinitializer, %unwind.map.entry zeroinitializer, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test4@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 6, %unwind.map.entry* getelementptr inbounds ([6 x %unwind.map.entry]* @"\01__unwindtable$test4@@YAHXZ", i32 0, i32 0), i32 3, %tryblock.map.entry* getelementptr inbounds ([3 x %tryblock.map.entry]* @"\01__tryblocktable$test4@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 5//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test5@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test5@@YAHXZ", %catch) }]

// CHECK: @"\01__catchsym$test5@@YAHXZ$1" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test5@@YAHXZ", %catch27) }]

// CHECK: @"\01__catchsym$test5@@YAHXZ$2" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test5@@YAHXZ", %catch30) }]

// CHECK: @"\01__tryblocktable$test5@@YAHXZ" = weak global [3 x %tryblock.map.entry] [%tryblock.map.entry { i32 1, i32 3, i32 4, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test5@@YAHXZ$0", i32 0, i32 0) }, %tryblock.map.entry { i32 5, i32 7, i32 8, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test5@@YAHXZ$1", i32 0, i32 0) }, %tryblock.map.entry { i32 0, i32 8, i32 9, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test5@@YAHXZ$2", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test5@@YAHXZ" = weak global [10 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 1, i8* blockaddress(@"\01?test5@@YAHXZ", %ehcleanup9) }, %unwind.map.entry { i32 2, i8* blockaddress(@"\01?test5@@YAHXZ", %ehcleanup) }, %unwind.map.entry zeroinitializer, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 5, i8* blockaddress(@"\01?test5@@YAHXZ", %ehcleanup23) }, %unwind.map.entry { i32 6, i8* blockaddress(@"\01?test5@@YAHXZ", %ehcleanup20) }, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test5@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 10, %unwind.map.entry* getelementptr inbounds ([10 x %unwind.map.entry]* @"\01__unwindtable$test5@@YAHXZ", i32 0, i32 0), i32 3, %tryblock.map.entry* getelementptr inbounds ([3 x %tryblock.map.entry]* @"\01__tryblocktable$test5@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 6//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test6@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %catch19) }]

// CHECK: @"\01__catchsym$test6@@YAHXZ$1" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %catch) }]

// CHECK: @"\01__catchsym$test6@@YAHXZ$2" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %catch43) }]

// CHECK: @"\01__catchsym$test6@@YAHXZ$3" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test6@@YAHXZ", %catch46) }]

// CHECK: @"\01__tryblocktable$test6@@YAHXZ" = weak global [4 x %tryblock.map.entry] [%tryblock.map.entry { i32 7, i32 7, i32 8, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test6@@YAHXZ$0", i32 0, i32 0) }, %tryblock.map.entry { i32 2, i32 4, i32 8, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test6@@YAHXZ$1", i32 0, i32 0) }, %tryblock.map.entry { i32 10, i32 12, i32 13, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test6@@YAHXZ$2", i32 0, i32 0) }, %tryblock.map.entry { i32 1, i32 13, i32 14, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test6@@YAHXZ$3", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test6@@YAHXZ" = weak global [15 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* blockaddress(@"\01?test6@@YAHXZ", %ehcleanup48) }, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 2, i8* blockaddress(@"\01?test6@@YAHXZ", %ehcleanup10) }, %unwind.map.entry { i32 3, i8* blockaddress(@"\01?test6@@YAHXZ", %ehcleanup) }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 5, i8* blockaddress(@"\01?test6@@YAHXZ", %ehcleanup25) }, %unwind.map.entry { i32 6, i8* null }, %unwind.map.entry { i32 6, i8* null }, %unwind.map.entry { i32 6, i8* null }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 10, i8* blockaddress(@"\01?test6@@YAHXZ", %ehcleanup39) }, %unwind.map.entry { i32 11, i8* blockaddress(@"\01?test6@@YAHXZ", %ehcleanup36) }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry zeroinitializer]

// CHECK: @"\01__ehfuncinfo$test6@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 15, %unwind.map.entry* getelementptr inbounds ([15 x %unwind.map.entry]* @"\01__unwindtable$test6@@YAHXZ", i32 0, i32 0), i32 4, %tryblock.map.entry* getelementptr inbounds ([4 x %tryblock.map.entry]* @"\01__tryblocktable$test6@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 7//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test7@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUA@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test7@@YAHXZ", %catch) }]

// CHECK: @"\01__tryblocktable$test7@@YAHXZ" = weak global [1 x %tryblock.map.entry] [%tryblock.map.entry { i32 0, i32 0, i32 1, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test7@@YAHXZ$0", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test7@@YAHXZ" = weak global [2 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test7@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 2, %unwind.map.entry* getelementptr inbounds ([2 x %unwind.map.entry]* @"\01__unwindtable$test7@@YAHXZ", i32 0, i32 0), i32 1, %tryblock.map.entry* getelementptr inbounds ([1 x %tryblock.map.entry]* @"\01__tryblocktable$test7@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }
//////////////////////////////////////////////////////////////////////////////////////////////////

// CHECK: @"\01__CT??_R0?AUD@@@81" = weak global %catchable.type { i32 0, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUD@@@8" to %type.descriptor*), %pmd.type { i32 0, i32 -1, i32 0 }, i32 1, void ()* null }

// CHECK: @"\01__CTA1?AUD@@" = weak global { i32, [1 x %catchable.type*] } { i32 1, [1 x %catchable.type*] [%catchable.type* @"\01__CT??_R0?AUD@@@81"] }

// CHECK: @"\01__TI1?AUD@@" = weak global %throw.info.type { i32 0, void ()* bitcast (void (%struct.D*)* @"\01??1D@@QAE@XZ" to void ()*), i32 ()* null, %catchable.array.type* bitcast ({ i32, [1 x %catchable.type*] }* @"\01__CTA1?AUD@@" to %catchable.array.type*) }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 8//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test8@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUD@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test8@@YAHXZ", %catch) }]

// CHECK: @"\01__tryblocktable$test8@@YAHXZ" = weak global [1 x %tryblock.map.entry] [%tryblock.map.entry { i32 0, i32 0, i32 1, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test8@@YAHXZ$0", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test8@@YAHXZ" = weak global [2 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test8@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 2, %unwind.map.entry* getelementptr inbounds ([2 x %unwind.map.entry]* @"\01__unwindtable$test8@@YAHXZ", i32 0, i32 0), i32 1, %tryblock.map.entry* getelementptr inbounds ([1 x %tryblock.map.entry]* @"\01__tryblocktable$test8@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 9//////////////////////////////////////////////

// CHECK: @"\01__catchsym$test9@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [8 x i8] }* @"\01??_R0?AUD@@@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test9@@YAHXZ", %catch) }]

// CHECK: @"\01__tryblocktable$test9@@YAHXZ" = weak global [1 x %tryblock.map.entry] [%tryblock.map.entry { i32 0, i32 1, i32 2, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test9@@YAHXZ$0", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test9@@YAHXZ" = weak global [3 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test9@@YAHXZ", %ehcleanup) }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test9@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 3, %unwind.map.entry* getelementptr inbounds ([3 x %unwind.map.entry]* @"\01__unwindtable$test9@@YAHXZ", i32 0, i32 0), i32 1, %tryblock.map.entry* getelementptr inbounds ([1 x %tryblock.map.entry]* @"\01__tryblocktable$test9@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 10/////////////////////////////////////////////

// CHECK: @"\01__catchsym$test10@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 0, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test10@@YAHXZ", %catch) }]

// CHECK: @"\01__catchsym$test10@@YAHXZ$1" = weak global [1 x %handler.type] [%handler.type { i32 0, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test10@@YAHXZ", %catch10) }]

// CHECK: @"\01__catchsym$test10@@YAHXZ$2" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test10@@YAHXZ", %catch5) }]

// CHECK: @"\01__tryblocktable$test10@@YAHXZ" = weak global [3 x %tryblock.map.entry] [%tryblock.map.entry { i32 1, i32 1, i32 2, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test10@@YAHXZ$0", i32 0, i32 0) }, %tryblock.map.entry { i32 4, i32 4, i32 5, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test10@@YAHXZ$1", i32 0, i32 0) }, %tryblock.map.entry { i32 0, i32 2, i32 5, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test10@@YAHXZ$2", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test10@@YAHXZ" = weak global [6 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry zeroinitializer, %unwind.map.entry zeroinitializer, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 3, i8* null }, %unwind.map.entry { i32 3, i8* null }]

// CHECK: @"\01__ehfuncinfo$test10@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 6, %unwind.map.entry* getelementptr inbounds ([6 x %unwind.map.entry]* @"\01__unwindtable$test10@@YAHXZ", i32 0, i32 0), i32 3, %tryblock.map.entry* getelementptr inbounds ([3 x %tryblock.map.entry]* @"\01__tryblocktable$test10@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 11/////////////////////////////////////////////

// CHECK: @"\01__catchsym$test11@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test11@@YAHXZ", %catch) }]

// CHECK: @"\01__tryblocktable$test11@@YAHXZ" = weak global [1 x %tryblock.map.entry] [%tryblock.map.entry { i32 0, i32 1, i32 2, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test11@@YAHXZ$0", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test11@@YAHXZ" = weak global [3 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 0, i8* blockaddress(@"\01?test11@@YAHXZ", %ehcleanup) }, %unwind.map.entry { i32 -1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test11@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 3, %unwind.map.entry* getelementptr inbounds ([3 x %unwind.map.entry]* @"\01__unwindtable$test11@@YAHXZ", i32 0, i32 0), i32 1, %tryblock.map.entry* getelementptr inbounds ([1 x %tryblock.map.entry]* @"\01__tryblocktable$test11@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////Test 12/////////////////////////////////////////////

// CHECK: @"\01__catchsym$test12@@YAHXZ$0" = weak global [1 x %handler.type] [%handler.type { i32 0, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test12@@YAHXZ", %catch9) }]

// CHECK: @"\01__catchsym$test12@@YAHXZ$1" = weak global [1 x %handler.type] [%handler.type { i32 8, %type.descriptor* bitcast ({ i8**, i32, [3 x i8] }* @"\01??_R0H@8" to %type.descriptor*), i32 0, i8* blockaddress(@"\01?test12@@YAHXZ", %catch) }]

// CHECK: @"\01__tryblocktable$test12@@YAHXZ" = weak global [2 x %tryblock.map.entry] [%tryblock.map.entry { i32 2, i32 3, i32 4, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test12@@YAHXZ$0", i32 0, i32 0) }, %tryblock.map.entry { i32 0, i32 0, i32 4, i32 1, %handler.type* getelementptr inbounds ([1 x %handler.type]* @"\01__catchsym$test12@@YAHXZ$1", i32 0, i32 0) }]

// CHECK: @"\01__unwindtable$test12@@YAHXZ" = weak global [5 x %unwind.map.entry] [%unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 -1, i8* null }, %unwind.map.entry { i32 1, i8* null }, %unwind.map.entry { i32 2, i8* blockaddress(@"\01?test12@@YAHXZ", %ehcleanup) }, %unwind.map.entry { i32 1, i8* null }]

// CHECK: @"\01__ehfuncinfo$test12@@YAHXZ" = weak global %ehfuncinfo { i32 429065506, i32 5, %unwind.map.entry* getelementptr inbounds ([5 x %unwind.map.entry]* @"\01__unwindtable$test12@@YAHXZ", i32 0, i32 0), i32 2, %tryblock.map.entry* getelementptr inbounds ([2 x %tryblock.map.entry]* @"\01__tryblocktable$test12@@YAHXZ", i32 0, i32 0), i32 0, i8* null, %estypelist* null, i32 1 }

//////////////////////////////////////////////////////////////////////////////////////////////////

struct A {
  A(){}
  ~A(){}
};


struct B {
  B(){}
  ~B(){}
};

struct C {
};

struct D{
  ~D(){}
};

void s() { throw 1; }
void sa() { throw A(); }

int test1() {
  try {
    A a, b;
  } catch (A &) {}
  return 0;
}

int test2() {
  try {
    s();
    try {
      s();
    } catch (B &) {}
  } catch (int &) {}
  return 0;
}

int test3() {
  try {
    s();
  } catch (int &) {
  }
  try {
    s();
  } catch (int &) {
  }
  return 0;
}

int test4() {
  int a;
  try {
    C fl;
    try {
      C a, b;
      s();
    } catch (int &) {
      a = 809;
    }
    C gl;
    try {
      C h, gh;
      s();
    } catch (int &) {
      a = 98;
    }
  } catch (C&) {}
  return 0;
}

int test5() {
  C ad;
  try {
    C fl;
    try {
      A a, b;
      s();
    } catch (int &) {}
    C gl;
    try {
      B h, gh;
      s();
    } catch (int &) {}
  } catch (int &) {}
  return 0;
}

int test6() {
  C ad;
  B ghj;
  try {
    C fl;
    try {
      A a, b;
      s();
    } catch (int &) {
      A sd;
      try {
        s();
      } catch (int&) {}
      B j;
    }
    C gl;
    try {
      B h, gh;
      s();
    } catch (int &) {}
  } catch (int &) {}
  C a;
  return 0;
}

int test7() {
  try {
    sa();
  } catch (A&) {
  }
  return 0;
}

int test8() {
  try {
    throw D();
  } catch (D&) {
  }
  return 0;
}

int test9() {
  try {
    D a;
    throw a;
  } catch (D&) {
  }
  return 0;
}

int test10() {
  try {
    try {
      s();
    } catch (int u) {
    }
    s();
  } catch (int &uga2) {
    try {
      s();
    } catch (int u) {
    }
  }
  return 0;
}

int test11() {
  try {
    A a;
    s();
  } catch (int &ex) {
  }
  return 0;
}

int test12() {
  try {
    s();
  } catch (int &uga2) {
    try {
      B b;
      s();
    } catch (int u) {
    }
  }
  return 0;
}
