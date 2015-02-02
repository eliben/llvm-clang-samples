; ModuleID = '<stdin>'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.mystruct = type { double, i8, i32, [10 x i8] }

; Function Attrs: nounwind uwtable
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
  %i12 = getelementptr inbounds %struct.mystruct* %b, i32 0, i32 2
  %7 = load i32* %i12, align 4
  %cmp = icmp ult i32 %6, %7
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %8 = load i32* %i11, align 4
  %idxprom = sext i32 %8 to i64
  %ar = getelementptr inbounds %struct.mystruct* %a, i32 0, i32 3
  %arrayidx = getelementptr inbounds [10 x i8]* %ar, i32 0, i64 %idxprom
  %9 = load i8* %arrayidx, align 1
  %conv14 = sext i8 %9 to i32
  %10 = load i32* %i11, align 4
  %idxprom15 = sext i32 %10 to i64
  %ar16 = getelementptr inbounds %struct.mystruct* %b, i32 0, i32 3
  %arrayidx17 = getelementptr inbounds [10 x i8]* %ar16, i32 0, i64 %idxprom15
  %11 = load i8* %arrayidx17, align 1
  %conv18 = sext i8 %11 to i32
  %add19 = add nsw i32 %conv14, %conv18
  %conv20 = trunc i32 %add19 to i8
  %12 = load i32* %i11, align 4
  %idxprom21 = sext i32 %12 to i64
  %ar22 = getelementptr inbounds %struct.mystruct* %ss, i32 0, i32 3
  %arrayidx23 = getelementptr inbounds [10 x i8]* %ar22, i32 0, i64 %idxprom21
  store i8 %conv20, i8* %arrayidx23, align 1
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %13 = load i32* %i11, align 4
  %inc = add nsw i32 %13, 1
  store i32 %inc, i32* %i11, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %14 = load %struct.mystruct** %out.addr, align 8
  %arrayidx24 = getelementptr inbounds %struct.mystruct* %14, i64 13
  %ar25 = getelementptr inbounds %struct.mystruct* %arrayidx24, i32 0, i32 3
  %arrayidx26 = getelementptr inbounds [10 x i8]* %ar25, i32 0, i64 2
  %15 = load i8* %arrayidx26, align 1
  %conv27 = sext i8 %15 to i32
  %add28 = add nsw i32 %conv27, 1
  %conv29 = trunc i32 %add28 to i8
  store i8 %conv29, i8* %arrayidx26, align 1
  %16 = load %struct.mystruct** %out.addr, align 8
  %17 = bitcast %struct.mystruct* %16 to i8*
  %18 = bitcast %struct.mystruct* %ss to i8*
  %call = call i8* @cpy(i8* %17, i8* %18, i32 32)
  ret void
}

declare i8* @cpy(i8*, i8*, i32) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.5.0 (trunk 209901)"}
