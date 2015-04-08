int foo(int* p, int v) {
  if (p == 0) {
    return v + 1;
  } else {
    return v - 1;
  }
}

