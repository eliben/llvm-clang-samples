; ModuleID = '<stdin>'
target datalayout = "e-i64:64-f80:128-s:64-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.uint3 = type { i32, i32, i32 }

@threadIdx = external constant %struct.uint3

; Function Attrs: nounwind uwtable
define i32 @foo(i32 %a, i32 %b) #0 {
entry:
  %retval = alloca i32, align 4
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  %0 = load i32* getelementptr inbounds (%struct.uint3* @threadIdx, i32 0, i32 0), align 4
  %cmp = icmp ugt i32 %0, 5
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %1 = load i32* %a.addr, align 4
  store i32 %1, i32* %retval
  br label %return

if.else:                                          ; preds = %entry
  %2 = load i32* %b.addr, align 4
  store i32 %2, i32* %retval
  br label %return

return:                                           ; preds = %if.else, %if.then
  %3 = load i32* %retval
  ret i32 %3
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.5 (trunk 197997)"}
