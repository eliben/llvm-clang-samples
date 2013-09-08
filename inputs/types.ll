; ModuleID = 'types.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.mystruct = type { double, i8, i32, [10 x i8] }

; Function Attrs: nounwind
define void @structadder(%struct.mystruct* byval align 8 %a, %struct.mystruct* byval align 8 %b, %struct.mystruct* %out) #0 {
entry:
  %out.addr = alloca %struct.mystruct*, align 8
  %ss = alloca %struct.mystruct, align 8
  %i11 = alloca i32, align 4
  store %struct.mystruct* %out, %struct.mystruct** %out.addr, align 8
  %d = getelementptr inbounds %struct.mystruct* %a, i32 0, i32 0
  %0 = load double* %d, align 8
  %d1 = getelementptr inbounds %struct.mystruct* %b, i32 0, i32 0
  %1 = load double* %d1, align 8
  %add = fadd double %0, %1
  %d2 = getelementptr inbounds %struct.mystruct* %ss, i32 0, i32 0
  store double %add, double* %d2, align 8
  %c = getelementptr inbounds %struct.mystruct* %a, i32 0, i32 1
  %2 = load i8* %c, align 1
  %conv = sext i8 %2 to i32
  %c3 = getelementptr inbounds %struct.mystruct* %b, i32 0, i32 1
  %3 = load i8* %c3, align 1
  %conv4 = sext i8 %3 to i32
  %add5 = add nsw i32 %conv, %conv4
  %conv6 = trunc i32 %add5 to i8
  %c7 = getelementptr inbounds %struct.mystruct* %ss, i32 0, i32 1
  store i8 %conv6, i8* %c7, align 1
  %i = getelementptr inbounds %struct.mystruct* %a, i32 0, i32 2
  %4 = load i32* %i, align 4
  %i8 = getelementptr inbounds %struct.mystruct* %b, i32 0, i32 2
  %5 = load i32* %i8, align 4
  %add9 = add i32 %4, %5
  %i10 = getelementptr inbounds %struct.mystruct* %ss, i32 0, i32 2
  store i32 %add9, i32* %i10, align 4
  store i32 0, i32* %i11, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %6 = load i32* %i11, align 4
  %cmp = icmp slt i32 %6, 10
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %7 = load i32* %i11, align 4
  %idxprom = sext i32 %7 to i64
  %ar = getelementptr inbounds %struct.mystruct* %a, i32 0, i32 3
  %arrayidx = getelementptr inbounds [10 x i8]* %ar, i32 0, i64 %idxprom
  %8 = load i8* %arrayidx, align 1
  %conv13 = sext i8 %8 to i32
  %9 = load i32* %i11, align 4
  %idxprom14 = sext i32 %9 to i64
  %ar15 = getelementptr inbounds %struct.mystruct* %b, i32 0, i32 3
  %arrayidx16 = getelementptr inbounds [10 x i8]* %ar15, i32 0, i64 %idxprom14
  %10 = load i8* %arrayidx16, align 1
  %conv17 = sext i8 %10 to i32
  %add18 = add nsw i32 %conv13, %conv17
  %conv19 = trunc i32 %add18 to i8
  %11 = load i32* %i11, align 4
  %idxprom20 = sext i32 %11 to i64
  %ar21 = getelementptr inbounds %struct.mystruct* %ss, i32 0, i32 3
  %arrayidx22 = getelementptr inbounds [10 x i8]* %ar21, i32 0, i64 %idxprom20
  store i8 %conv19, i8* %arrayidx22, align 1
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %12 = load i32* %i11, align 4
  %inc = add nsw i32 %12, 1
  store i32 %inc, i32* %i11, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %13 = load %struct.mystruct** %out.addr, align 8
  %14 = bitcast %struct.mystruct* %13 to i8*
  %15 = bitcast %struct.mystruct* %ss to i8*
  %call = call i8* @cpy(i8* %14, i8* %15, i32 32)
  ret void
}

declare i8* @cpy(i8*, i8*, i32) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
