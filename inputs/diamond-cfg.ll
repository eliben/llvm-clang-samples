; Crafted to have a diamond-like CFG for testing the CFG analysis
; samples.

define i32 @func(i32 %a, i32 %b) #0 {
AA:
  %cmp1 = icmp eq i32 %a, %b
  br i1 %cmp1, label %BB, label %CC

BB:
  %cmp2 = icmp ult i32 %a, %b
  br i1 %cmp2, label %CC, label %DD

CC:
  br label %DD

DD:
  ret i32 %a
}

