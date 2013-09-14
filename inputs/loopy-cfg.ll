; Has a number of distinct loops in an otherwise simple graph

define i32 @func(i32 %a, i32 %b) #0 {
AA:
  %cmp1 = icmp eq i32 %a, %b
  br i1 %cmp1, label %BB, label %CC

BB:
  br label %BB1

BB1:
  br label %BB2

BB2:
  %cmp4 = icmp eq i32 %a, %b
  br i1 %cmp4, label %BB3, label %BB

BB3:
  br label %BB4

BB4:
  %cmp3 = icmp eq i32 %a, %b
  br i1 %cmp3, label %BB3, label %CC

CC:
  br label %CC1

CC1:
  br label %CC2

CC2:
  %cmp2 = icmp eq i32 %a, %b
  br i1 %cmp2, label %DD, label %CC

DD:
  ret i32 %a
}


