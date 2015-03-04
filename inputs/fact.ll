; ModuleID = 'fact.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind
define i32 @mult(i32 %a, i32 %b) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %i = alloca i32, align 4
  %s = alloca i32, align 4
  store i32 %a, i32* %1, align 4
  store i32 %b, i32* %2, align 4
  store i32 0, i32* %s, align 4
  store i32 0, i32* %i, align 4
  br label %3

; <label>:3                                       ; preds = %11, %0
  %4 = load i32, i32* %i, align 4
  %5 = load i32, i32* %1, align 4
  %6 = icmp ult i32 %4, %5
  br i1 %6, label %7, label %14

; <label>:7                                       ; preds = %3
  %8 = load i32, i32* %2, align 4
  %9 = load i32, i32* %s, align 4
  %10 = add i32 %9, %8
  store i32 %10, i32* %s, align 4
  br label %11

; <label>:11                                      ; preds = %7
  %12 = load i32, i32* %i, align 4
  %13 = add i32 %12, 1
  store i32 %13, i32* %i, align 4
  br label %3

; <label>:14                                      ; preds = %3
  %15 = load i32, i32* %s, align 4
  ret i32 %15
}

; Function Attrs: nounwind
define i32 @fact(i32 %a) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 %a, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = icmp ule i32 %3, 1
  br i1 %4, label %5, label %6

; <label>:5                                       ; preds = %0
  store i32 1, i32* %1
  br label %12

; <label>:6                                       ; preds = %0
  %7 = load i32, i32* %2, align 4
  %8 = load i32, i32* %2, align 4
  %9 = sub i32 %8, 1
  %10 = call i32 @fact(i32 %9)
  %11 = mul i32 %7, %10
  store i32 %11, i32* %1
  br label %12

; <label>:12                                      ; preds = %6, %5
  %13 = load i32, i32* %1
  ret i32 %13
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.5 (199390)"}
