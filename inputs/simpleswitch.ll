; ModuleID = 'inputs/simpleswitch.c'
source_filename = "inputs/simpleswitch.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind
define i32 @decide(i8 zeroext %pred, i32* %a) #0 {
  %retval = alloca i32, align 4
  %pred.addr = alloca i8, align 1
  %a.addr = alloca i32*, align 8
  store i8 %pred, i8* %pred.addr, align 1
  store i32* %a, i32** %a.addr, align 8
  %1 = load i8, i8* %pred.addr, align 1
  %conv = zext i8 %1 to i32
  %2 = load i32*, i32** %a.addr, align 8
  %arrayidx = getelementptr inbounds i32, i32* %2, i64 2
  %3 = load i32, i32* %arrayidx, align 4
  %cmp = icmp ugt i32 %conv, %3
  br i1 %cmp, label %4, label %7

; <label>:4:                                      ; preds = %0
  %5 = load i32*, i32** %a.addr, align 8
  %arrayidx2 = getelementptr inbounds i32, i32* %5, i64 3
  %6 = load i32, i32* %arrayidx2, align 4
  store i32 %6, i32* %retval, align 4
  br label %30

; <label>:7:                                      ; preds = %0
  %8 = load i8, i8* %pred.addr, align 1
  %conv3 = zext i8 %8 to i32
  switch i32 %conv3, label %24 [
    i32 1, label %9
    i32 2, label %14
    i32 200, label %19
  ]

; <label>:9:                                      ; preds = %7
  %10 = load i32*, i32** %a.addr, align 8
  %arrayidx4 = getelementptr inbounds i32, i32* %10, i64 0
  %11 = load i32, i32* %arrayidx4, align 4
  %12 = load i32*, i32** %a.addr, align 8
  %arrayidx5 = getelementptr inbounds i32, i32* %12, i64 2
  %13 = load i32, i32* %arrayidx5, align 4
  %add = add i32 %11, %13
  store i32 %add, i32* %retval, align 4
  br label %30

; <label>:14:                                     ; preds = %7
  %15 = load i32*, i32** %a.addr, align 8
  %arrayidx6 = getelementptr inbounds i32, i32* %15, i64 3
  %16 = load i32, i32* %arrayidx6, align 4
  %17 = load i32*, i32** %a.addr, align 8
  %arrayidx7 = getelementptr inbounds i32, i32* %17, i64 4
  %18 = load i32, i32* %arrayidx7, align 4
  %sub = sub i32 %16, %18
  store i32 %sub, i32* %retval, align 4
  br label %30

; <label>:19:                                     ; preds = %7
  %20 = load i32*, i32** %a.addr, align 8
  %arrayidx8 = getelementptr inbounds i32, i32* %20, i64 5
  %21 = load i32, i32* %arrayidx8, align 4
  %22 = load i32*, i32** %a.addr, align 8
  %arrayidx9 = getelementptr inbounds i32, i32* %22, i64 6
  %23 = load i32, i32* %arrayidx9, align 4
  %mul = mul i32 %21, %23
  store i32 %mul, i32* %retval, align 4
  br label %30

; <label>:24:                                     ; preds = %7
  br label %25

; <label>:25:                                     ; preds = %24
  %26 = load i32*, i32** %a.addr, align 8
  %arrayidx10 = getelementptr inbounds i32, i32* %26, i64 9
  %27 = load i32, i32* %arrayidx10, align 4
  %28 = load i32*, i32** %a.addr, align 8
  %arrayidx11 = getelementptr inbounds i32, i32* %28, i64 4
  %29 = load i32, i32* %arrayidx11, align 4
  %mul12 = mul i32 %27, %29
  store i32 %mul12, i32* %retval, align 4
  br label %30

; <label>:30:                                     ; preds = %25, %19, %14, %9, %4
  %31 = load i32, i32* %retval, align 4
  ret i32 %31
}

attributes #0 = { noinline nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-features"="+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 5.0.0 (trunk 295993)"}
