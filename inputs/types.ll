; ModuleID = '<stdin>'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.mystruct = type { double, i8, i32, [10 x i8] }

; Function Attrs: nounwind uwtable
define void @structadder(%struct.mystruct* byval nocapture readonly align 8 %a, %struct.mystruct* byval nocapture readonly align 8 %b, %struct.mystruct* %out) #0 {
entry:
  %ss = alloca %struct.mystruct, align 8
  %d = getelementptr inbounds %struct.mystruct, %struct.mystruct* %a, i64 0, i32 0
  %0 = load double, double* %d, align 8, !tbaa !1
  %d1 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %b, i64 0, i32 0
  %1 = load double, double* %d1, align 8, !tbaa !1
  %add = fadd double %0, %1
  %d2 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %ss, i64 0, i32 0
  store double %add, double* %d2, align 8, !tbaa !1
  %c = getelementptr inbounds %struct.mystruct, %struct.mystruct* %a, i64 0, i32 1
  %2 = bitcast i8* %c to i64*
  %3 = load i64, i64* %2, align 8
  %4 = trunc i64 %3 to i8
  %c3 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %b, i64 0, i32 1
  %5 = bitcast i8* %c3 to i64*
  %6 = load i64, i64* %5, align 8
  %7 = trunc i64 %6 to i8
  %add5 = add i8 %7, %4
  %c7 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %ss, i64 0, i32 1
  store i8 %add5, i8* %c7, align 8, !tbaa !7
  %8 = lshr i64 %3, 32
  %9 = trunc i64 %8 to i32
  %10 = lshr i64 %6, 32
  %11 = trunc i64 %10 to i32
  %add9 = add i32 %11, %9
  %i10 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %ss, i64 0, i32 2
  store i32 %add9, i32* %i10, align 4, !tbaa !8
  %cmp40 = icmp eq i32 %11, 0
  br i1 %cmp40, label %for.end, label %overflow.checked

overflow.checked:                                 ; preds = %entry
  %12 = lshr i64 %6, 32
  %13 = add nuw nsw i64 %12, 4294967295
  %14 = and i64 %13, 4294967295
  %15 = add nuw nsw i64 %14, 1
  %end.idx = add nuw nsw i64 %14, 1
  %n.mod.vf = and i64 %12, 31
  %n.vec = sub nsw i64 %15, %n.mod.vf
  %cmp.zero = icmp eq i64 %15, %n.mod.vf
  br i1 %cmp.zero, label %middle.block, label %vector.body.preheader

vector.body.preheader:                            ; preds = %overflow.checked
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.body.preheader
  %index = phi i64 [ %index.next, %vector.body ], [ 0, %vector.body.preheader ]
  %16 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %a, i64 0, i32 3, i64 %index
  %17 = bitcast i8* %16 to <16 x i8>*
  %wide.load = load <16 x i8>, <16 x i8>* %17, align 8, !tbaa !9
  %.sum49 = or i64 %index, 16
  %18 = getelementptr %struct.mystruct, %struct.mystruct* %a, i64 0, i32 3, i64 %.sum49
  %19 = bitcast i8* %18 to <16 x i8>*
  %wide.load43 = load <16 x i8>, <16 x i8>* %19, align 8, !tbaa !9
  %20 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %b, i64 0, i32 3, i64 %index
  %21 = bitcast i8* %20 to <16 x i8>*
  %wide.load44 = load <16 x i8>, <16 x i8>* %21, align 8, !tbaa !9
  %.sum50 = or i64 %index, 16
  %22 = getelementptr %struct.mystruct, %struct.mystruct* %b, i64 0, i32 3, i64 %.sum50
  %23 = bitcast i8* %22 to <16 x i8>*
  %wide.load45 = load <16 x i8>, <16 x i8>* %23, align 8, !tbaa !9
  %24 = add <16 x i8> %wide.load44, %wide.load
  %25 = add <16 x i8> %wide.load45, %wide.load43
  %26 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %ss, i64 0, i32 3, i64 %index
  %27 = bitcast i8* %26 to <16 x i8>*
  store <16 x i8> %24, <16 x i8>* %27, align 8, !tbaa !9
  %.sum51 = or i64 %index, 16
  %28 = getelementptr %struct.mystruct, %struct.mystruct* %ss, i64 0, i32 3, i64 %.sum51
  %29 = bitcast i8* %28 to <16 x i8>*
  store <16 x i8> %25, <16 x i8>* %29, align 8, !tbaa !9
  %index.next = add i64 %index, 32
  %30 = icmp eq i64 %index.next, %n.vec
  br i1 %30, label %middle.block.loopexit, label %vector.body, !llvm.loop !10

middle.block.loopexit:                            ; preds = %vector.body
  br label %middle.block

middle.block:                                     ; preds = %middle.block.loopexit, %overflow.checked
  %resume.val = phi i64 [ 0, %overflow.checked ], [ %n.vec, %middle.block.loopexit ]
  %cmp.n = icmp eq i64 %end.idx, %resume.val
  br i1 %cmp.n, label %for.end, label %for.body.preheader

for.body.preheader:                               ; preds = %middle.block
  %31 = lshr i64 %6, 32
  %32 = trunc i64 %31 to i32
  %33 = add i64 %resume.val, 1
  %34 = trunc i64 %33 to i32
  %35 = icmp ugt i32 %32, %34
  %umax = select i1 %35, i32 %32, i32 %34
  %36 = add i32 %umax, 1
  %37 = sub i32 %36, %34
  %38 = sub i32 %umax, %34
  %xtraiter = and i32 %37, 1
  %lcmp.mod = icmp ne i32 %xtraiter, 0
  br i1 %lcmp.mod, label %for.body.prol, label %for.body.preheader.split

for.body.prol:                                    ; preds = %for.body.preheader
  %arrayidx.prol = getelementptr inbounds %struct.mystruct, %struct.mystruct* %a, i64 0, i32 3, i64 %resume.val
  %39 = load i8, i8* %arrayidx.prol, align 1, !tbaa !9
  %arrayidx17.prol = getelementptr inbounds %struct.mystruct, %struct.mystruct* %b, i64 0, i32 3, i64 %resume.val
  %40 = load i8, i8* %arrayidx17.prol, align 1, !tbaa !9
  %add19.prol = add i8 %40, %39
  %arrayidx23.prol = getelementptr inbounds %struct.mystruct, %struct.mystruct* %ss, i64 0, i32 3, i64 %resume.val
  store i8 %add19.prol, i8* %arrayidx23.prol, align 1, !tbaa !9
  %indvars.iv.next.prol = add nuw nsw i64 %resume.val, 1
  %41 = trunc i64 %indvars.iv.next.prol to i32
  %cmp.prol = icmp ult i32 %41, %11
  br label %for.body.preheader.split

for.body.preheader.split:                         ; preds = %for.body.prol, %for.body.preheader
  %indvars.iv.unr = phi i64 [ %resume.val, %for.body.preheader ], [ %indvars.iv.next.prol, %for.body.prol ]
  %42 = icmp ult i32 %38, 1
  br i1 %42, label %for.end.loopexit, label %for.body.preheader.split.split

for.body.preheader.split.split:                   ; preds = %for.body.preheader.split
  br label %for.body

for.body:                                         ; preds = %for.body, %for.body.preheader.split.split
  %indvars.iv = phi i64 [ %indvars.iv.unr, %for.body.preheader.split.split ], [ %indvars.iv.next.1, %for.body ]
  %arrayidx = getelementptr inbounds %struct.mystruct, %struct.mystruct* %a, i64 0, i32 3, i64 %indvars.iv
  %43 = load i8, i8* %arrayidx, align 1, !tbaa !9
  %arrayidx17 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %b, i64 0, i32 3, i64 %indvars.iv
  %44 = load i8, i8* %arrayidx17, align 1, !tbaa !9
  %add19 = add i8 %44, %43
  %arrayidx23 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %ss, i64 0, i32 3, i64 %indvars.iv
  store i8 %add19, i8* %arrayidx23, align 1, !tbaa !9
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %45 = trunc i64 %indvars.iv.next to i32
  %arrayidx.1 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %a, i64 0, i32 3, i64 %indvars.iv.next
  %46 = load i8, i8* %arrayidx.1, align 1, !tbaa !9
  %arrayidx17.1 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %b, i64 0, i32 3, i64 %indvars.iv.next
  %47 = load i8, i8* %arrayidx17.1, align 1, !tbaa !9
  %add19.1 = add i8 %47, %46
  %arrayidx23.1 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %ss, i64 0, i32 3, i64 %indvars.iv.next
  store i8 %add19.1, i8* %arrayidx23.1, align 1, !tbaa !9
  %indvars.iv.next.1 = add nuw nsw i64 %indvars.iv.next, 1
  %48 = trunc i64 %indvars.iv.next.1 to i32
  %cmp.1 = icmp ult i32 %48, %11
  br i1 %cmp.1, label %for.body, label %for.end.loopexit.unr-lcssa, !llvm.loop !13

for.end.loopexit.unr-lcssa:                       ; preds = %for.body
  br label %for.end.loopexit

for.end.loopexit:                                 ; preds = %for.end.loopexit.unr-lcssa, %for.body.preheader.split
  br label %for.end

for.end:                                          ; preds = %for.end.loopexit, %middle.block, %entry
  %arrayidx26 = getelementptr inbounds %struct.mystruct, %struct.mystruct* %out, i64 13, i32 3, i64 2
  %49 = load i8, i8* %arrayidx26, align 1, !tbaa !9
  %add28 = add i8 %49, 1
  store i8 %add28, i8* %arrayidx26, align 1, !tbaa !9
  %50 = bitcast %struct.mystruct* %out to i8*
  %51 = bitcast %struct.mystruct* %ss to i8*
  %call = call i8* @cpy(i8* %50, i8* %51, i32 32) #2
  ret void
}

declare i8* @cpy(i8*, i8*, i32) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.7.0 (http://llvm.org/git/clang 805ac0a233be64ffeeff2334db1873760c1588e9) (http://llvm.org/git/llvm.git 5109ec06a641f039775e1a6bcd7d50bb04dbb8f6)"}
!1 = !{!2, !3, i64 0}
!2 = !{!"", !3, i64 0, !4, i64 8, !6, i64 12, !4, i64 16}
!3 = !{!"double", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!"int", !4, i64 0}
!7 = !{!2, !4, i64 8}
!8 = !{!2, !6, i64 12}
!9 = !{!4, !4, i64 0}
!10 = distinct !{!10, !11, !12}
!11 = !{!"llvm.loop.vectorize.width", i32 1}
!12 = !{!"llvm.loop.interleave.count", i32 1}
!13 = distinct !{!13, !11, !12}
