unsigned mult(unsigned a, unsigned b) {
  unsigned i, s = 0;
  for (i = 0; i < a; ++i) {
    s += b;
  }
  return s;
}

unsigned fact(unsigned a) {
  if (a <= 1) {
    return 1;
  } else {
    return a * fact(a - 1);
  }
}
