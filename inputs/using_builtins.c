// This code uses printf without declaring.
//
// This will emit a warning and produce a declaration for the builtin
// implicitly:
//
//   clang -cc1 -emit-llvm inputs/using_builtins.c
//
// It will also produce a warning about mismatch in %d format with 'f' argument.
//
// This (C++ mode) will error, because in C++ such implicit declarations are not
// legal:
//
//   clang -cc1 -x c++ -emit-llvm  inputs/using_builtins.c
//

void foo(int i, float f) {
  printf("%d %f\n", f, f);
}
