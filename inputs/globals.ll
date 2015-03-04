; ModuleID = '<stdin>'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@gfloat = common global float 0.000000e+00, align 4
@gppfloat = common global float** null, align 8
@gint = common global i32 0, align 4

; Function Attrs: nounwind readonly uwtable
define float @foo() #0 {
entry:
  %0 = load float, float* @gfloat, align 4, !tbaa !1
  %1 = load float**, float*** @gppfloat, align 8, !tbaa !5
  %2 = load float*, float** %1, align 8, !tbaa !5
  %3 = load float, float* %2, align 4, !tbaa !1
  %add = fadd float %0, %3
  ret float %add
}

attributes #0 = { nounwind readonly uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.7.0 (http://llvm.org/git/clang 805ac0a233be64ffeeff2334db1873760c1588e9) (http://llvm.org/git/llvm.git 5109ec06a641f039775e1a6bcd7d50bb04dbb8f6)"}
!1 = !{!2, !2, i64 0}
!2 = !{!"float", !3, i64 0}
!3 = !{!"omnipotent char", !4, i64 0}
!4 = !{!"Simple C/C++ TBAA"}
!5 = !{!6, !6, i64 0}
!6 = !{!"any pointer", !3, i64 0}
