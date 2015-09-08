; ModuleID = 'inputs/twodoublefoo.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind readnone uwtable
define double @foo(double %a, double %b) #0 {
  %1 = fmul double %a, 2.000000e+00
  %2 = fadd double %1, %b
  ret double %2
}

; Function Attrs: nounwind readnone uwtable
define double @bar(double %a, double %b) #0 {
  %1 = tail call fastcc double @internal(double %a, double %b)
  %2 = fmul double %1, 1.000000e+01
  ret double %2
}

; Function Attrs: noinline nounwind readnone uwtable
define internal fastcc double @internal(double %a, double %b) #1 {
  %1 = fmul double %a, %b
  ret double %1
}

attributes #0 = { nounwind readnone uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noinline nounwind readnone uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.7.0 (tags/RELEASE_370/final)"}
