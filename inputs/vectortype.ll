; Example of using long vector types with arithmetic.
;
; llc -O3 inputs/vectortype.ll -o -
;
; also try with -mattr=avx2 for leveraging AVX2 instructions.

define <512 x i32> @add_vecs(<512 x i32> %a, <512 x i32> %b) {
   %1 = add <512 x i32> %a,  %b
   ret <512 x i32> %1
}
