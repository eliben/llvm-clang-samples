; ModuleID = 'inputs/fact.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind
define i32 @mult(i32 %a, i32 %b) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %i = alloca i32, align 4
  %s = alloca i32, align 4
  store i32 %a, i32* %1, align 4
  call void @llvm.dbg.declare(metadata !{i32* %1}, metadata !13), !dbg !14
  store i32 %b, i32* %2, align 4
  call void @llvm.dbg.declare(metadata !{i32* %2}, metadata !15), !dbg !14
  call void @llvm.dbg.declare(metadata !{i32* %i}, metadata !16), !dbg !17
  call void @llvm.dbg.declare(metadata !{i32* %s}, metadata !18), !dbg !17
  store i32 0, i32* %s, align 4, !dbg !17
  store i32 0, i32* %i, align 4, !dbg !19
  br label %3, !dbg !19

; <label>:3                                       ; preds = %11, %0
  %4 = load i32* %i, align 4, !dbg !19
  %5 = load i32* %1, align 4, !dbg !19
  %6 = icmp ult i32 %4, %5, !dbg !19
  br i1 %6, label %7, label %14, !dbg !19

; <label>:7                                       ; preds = %3
  %8 = load i32* %2, align 4, !dbg !21
  %9 = load i32* %s, align 4, !dbg !21
  %10 = add i32 %9, %8, !dbg !21
  store i32 %10, i32* %s, align 4, !dbg !21
  br label %11, !dbg !23

; <label>:11                                      ; preds = %7
  %12 = load i32* %i, align 4, !dbg !19
  %13 = add i32 %12, 1, !dbg !19
  store i32 %13, i32* %i, align 4, !dbg !19
  br label %3, !dbg !19

; <label>:14                                      ; preds = %3
  %15 = load i32* %s, align 4, !dbg !24
  ret i32 %15, !dbg !24
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata) #1

; Function Attrs: nounwind
define i32 @fact(i32 %a) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 %a, i32* %2, align 4
  call void @llvm.dbg.declare(metadata !{i32* %2}, metadata !25), !dbg !26
  %3 = load i32* %2, align 4, !dbg !27
  %4 = icmp ule i32 %3, 1, !dbg !27
  br i1 %4, label %5, label %6, !dbg !27

; <label>:5                                       ; preds = %0
  store i32 1, i32* %1, !dbg !28
  br label %12, !dbg !28

; <label>:6                                       ; preds = %0
  %7 = load i32* %2, align 4, !dbg !30
  %8 = load i32* %2, align 4, !dbg !30
  %9 = sub i32 %8, 1, !dbg !30
  %10 = call i32 @fact(i32 %9), !dbg !30
  %11 = mul i32 %7, %10, !dbg !30
  store i32 %11, i32* %1, !dbg !30
  br label %12, !dbg !30

; <label>:12                                      ; preds = %6, %5
  %13 = load i32* %1, !dbg !32
  ret i32 %13, !dbg !32
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-frame-pointer-elim-non-leaf"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }

!llvm.dbg.cu = !{!0}

!0 = metadata !{i32 786449, metadata !1, i32 12, metadata !"clang version 3.3 (tags/RELEASE_33/final)", i1 false, metadata !"", i32 0, metadata !2, metadata !2, metadata !3, metadata !2, metadata !2, metadata !""} ; [ DW_TAG_compile_unit ] [/usr/local/google/home/eliben/eli/llvm-clang-samples/inputs/<unknown>] [DW_LANG_C99]
!1 = metadata !{metadata !"inputs/<unknown>", metadata !"/usr/local/google/home/eliben/eli/llvm-clang-samples"}
!2 = metadata !{i32 0}
!3 = metadata !{metadata !4, metadata !10}
!4 = metadata !{i32 786478, metadata !5, metadata !6, metadata !"mult", metadata !"mult", metadata !"", i32 1, metadata !7, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, i32 (i32, i32)* @mult, null, null, metadata !2, i32 1} ; [ DW_TAG_subprogram ] [line 1] [def] [mult]
!5 = metadata !{metadata !"inputs/fact.c", metadata !"/usr/local/google/home/eliben/eli/llvm-clang-samples"}
!6 = metadata !{i32 786473, metadata !5}          ; [ DW_TAG_file_type ] [/usr/local/google/home/eliben/eli/llvm-clang-samples/inputs/fact.c]
!7 = metadata !{i32 786453, i32 0, i32 0, metadata !"", i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !8, i32 0, i32 0} ; [ DW_TAG_subroutine_type ] [line 0, size 0, align 0, offset 0] [from ]
!8 = metadata !{metadata !9, metadata !9, metadata !9}
!9 = metadata !{i32 786468, null, null, metadata !"unsigned int", i32 0, i64 32, i64 32, i64 0, i32 0, i32 7} ; [ DW_TAG_base_type ] [unsigned int] [line 0, size 32, align 32, offset 0, enc DW_ATE_unsigned]
!10 = metadata !{i32 786478, metadata !5, metadata !6, metadata !"fact", metadata !"fact", metadata !"", i32 9, metadata !11, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, i32 (i32)* @fact, null, null, metadata !2, i32 9} ; [ DW_TAG_subprogram ] [line 9] [def] [fact]
!11 = metadata !{i32 786453, i32 0, i32 0, metadata !"", i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !12, i32 0, i32 0} ; [ DW_TAG_subroutine_type ] [line 0, size 0, align 0, offset 0] [from ]
!12 = metadata !{metadata !9, metadata !9}
!13 = metadata !{i32 786689, metadata !4, metadata !"a", metadata !6, i32 16777217, metadata !9, i32 0, i32 0} ; [ DW_TAG_arg_variable ] [a] [line 1]
!14 = metadata !{i32 1, i32 0, metadata !4, null}
!15 = metadata !{i32 786689, metadata !4, metadata !"b", metadata !6, i32 33554433, metadata !9, i32 0, i32 0} ; [ DW_TAG_arg_variable ] [b] [line 1]
!16 = metadata !{i32 786688, metadata !4, metadata !"i", metadata !6, i32 2, metadata !9, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [i] [line 2]
!17 = metadata !{i32 2, i32 0, metadata !4, null}
!18 = metadata !{i32 786688, metadata !4, metadata !"s", metadata !6, i32 2, metadata !9, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [s] [line 2]
!19 = metadata !{i32 3, i32 0, metadata !20, null}
!20 = metadata !{i32 786443, metadata !5, metadata !4, i32 3, i32 0, i32 0} ; [ DW_TAG_lexical_block ] [/usr/local/google/home/eliben/eli/llvm-clang-samples/inputs/fact.c]
!21 = metadata !{i32 4, i32 0, metadata !22, null}
!22 = metadata !{i32 786443, metadata !5, metadata !20, i32 3, i32 0, i32 1} ; [ DW_TAG_lexical_block ] [/usr/local/google/home/eliben/eli/llvm-clang-samples/inputs/fact.c]
!23 = metadata !{i32 5, i32 0, metadata !22, null}
!24 = metadata !{i32 6, i32 0, metadata !4, null}
!25 = metadata !{i32 786689, metadata !10, metadata !"a", metadata !6, i32 16777225, metadata !9, i32 0, i32 0} ; [ DW_TAG_arg_variable ] [a] [line 9]
!26 = metadata !{i32 9, i32 0, metadata !10, null}
!27 = metadata !{i32 10, i32 0, metadata !10, null}
!28 = metadata !{i32 11, i32 0, metadata !29, null}
!29 = metadata !{i32 786443, metadata !5, metadata !10, i32 10, i32 0, i32 2} ; [ DW_TAG_lexical_block ] [/usr/local/google/home/eliben/eli/llvm-clang-samples/inputs/fact.c]
!30 = metadata !{i32 13, i32 0, metadata !31, null}
!31 = metadata !{i32 786443, metadata !5, metadata !10, i32 12, i32 0, i32 3} ; [ DW_TAG_lexical_block ] [/usr/local/google/home/eliben/eli/llvm-clang-samples/inputs/fact.c]
!32 = metadata !{i32 15, i32 0, metadata !10, null}
