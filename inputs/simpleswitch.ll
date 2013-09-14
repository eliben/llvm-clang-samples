; ModuleID = 'inputs/simpleswitch.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind
define i32 @decide(i8 zeroext %pred, i32* %a) #0 {
entry:
  %retval = alloca i32, align 4
  %pred.addr = alloca i8, align 1
  %a.addr = alloca i32*, align 8
  store i8 %pred, i8* %pred.addr, align 1
  store i32* %a, i32** %a.addr, align 8
  %0 = load i8* %pred.addr, align 1
  %conv = zext i8 %0 to i32
  %1 = load i32** %a.addr, align 8
  %arrayidx = getelementptr inbounds i32* %1, i64 2
  %2 = load i32* %arrayidx, align 4
  %cmp = icmp ugt i32 %conv, %2
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %3 = load i32** %a.addr, align 8
  %arrayidx2 = getelementptr inbounds i32* %3, i64 3
  %4 = load i32* %arrayidx2, align 4
  store i32 %4, i32* %retval
  br label %return

if.end:                                           ; preds = %entry
  %5 = load i8* %pred.addr, align 1
  %conv3 = zext i8 %5 to i32
  switch i32 %conv3, label %sw.default [
    i32 1, label %sw.bb
    i32 2, label %sw.bb6
    i32 200, label %sw.bb9
  ]

sw.bb:                                            ; preds = %if.end
  %6 = load i32** %a.addr, align 8
  %arrayidx4 = getelementptr inbounds i32* %6, i64 0
  %7 = load i32* %arrayidx4, align 4
  %8 = load i32** %a.addr, align 8
  %arrayidx5 = getelementptr inbounds i32* %8, i64 2
  %9 = load i32* %arrayidx5, align 4
  %add = add i32 %7, %9
  store i32 %add, i32* %retval
  br label %return

sw.bb6:                                           ; preds = %if.end
  %10 = load i32** %a.addr, align 8
  %arrayidx7 = getelementptr inbounds i32* %10, i64 3
  %11 = load i32* %arrayidx7, align 4
  %12 = load i32** %a.addr, align 8
  %arrayidx8 = getelementptr inbounds i32* %12, i64 4
  %13 = load i32* %arrayidx8, align 4
  %sub = sub i32 %11, %13
  store i32 %sub, i32* %retval
  br label %return

sw.bb9:                                           ; preds = %if.end
  %14 = load i32** %a.addr, align 8
  %arrayidx10 = getelementptr inbounds i32* %14, i64 5
  %15 = load i32* %arrayidx10, align 4
  %16 = load i32** %a.addr, align 8
  %arrayidx11 = getelementptr inbounds i32* %16, i64 6
  %17 = load i32* %arrayidx11, align 4
  %mul = mul i32 %15, %17
  store i32 %mul, i32* %retval
  br label %return

sw.default:                                       ; preds = %if.end
  br label %sw.epilog

sw.epilog:                                        ; preds = %sw.default
  %18 = load i32** %a.addr, align 8
  %arrayidx12 = getelementptr inbounds i32* %18, i64 9
  %19 = load i32* %arrayidx12, align 4
  %20 = load i32** %a.addr, align 8
  %arrayidx13 = getelementptr inbounds i32* %20, i64 4
  %21 = load i32* %arrayidx13, align 4
  %mul14 = mul i32 %19, %21
  store i32 %mul14, i32* %retval
  br label %return

return:                                           ; preds = %sw.epilog, %sw.bb9, %sw.bb6, %sw.bb, %if.then
  %22 = load i32* %retval
  ret i32 %22
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
