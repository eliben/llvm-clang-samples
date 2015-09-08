double foo(double a, double b) {
  return 2 * a + b;
}

__attribute__((noinline)) static double internal(double a, double b) {
  return a * b;
}

double bar(double a, double b) {
  return internal(a, b) * 10;
}
