; ModuleID = 'globals.c'
target datalayout = "e-i64:64-f80:128-s:64-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@gfloat = common global float 0.000000e+00, align 4
@gppfloat = common global float** null, align 8
@gint = common global i32 0, align 4

; Function Attrs: nounwind
define float @foo() #0 {
entry:
  %0 = load float* @gfloat, align 4
  %1 = load float*** @gppfloat, align 8
  %2 = load float** %1, align 8
  %3 = load float* %2, align 4
  %add = fadd float %0, %3
  ret float %add
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.5 (trunk 197997)"}
