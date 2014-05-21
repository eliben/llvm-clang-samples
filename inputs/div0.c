#define DODIV(a, b) ((a) / (b))

int test(int z) {
  if (z == 0) {
#ifdef FOO
    return DODIV(1, z);
#else
    return 1 - z;
#endif
  }
  return 1 + z;
}
