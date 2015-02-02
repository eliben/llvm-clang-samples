; ModuleID = '<stdin>'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.uint3 = type { i32, i32, i32 }

@threadIdx = external constant %struct.uint3

; Function Attrs: nounwind readnone uwtable
define i32 @foo(i32 %a, i32 %b) #0 {
entry:
  %0 = load i32* getelementptr inbounds (%struct.uint3* @threadIdx, i64 0, i32 1), align 4, !tbaa !1
  %cmp = icmp ugt i32 %0, 5
  %a.b = select i1 %cmp, i32 %a, i32 %b
  ret i32 %a.b
}

attributes #0 = { nounwind readnone uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.5 "}
!1 = !{!2, !3, i64 4}
!2 = !{!"", !3, i64 0, !3, i64 4, !3, i64 8}
!3 = !{!"int", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
