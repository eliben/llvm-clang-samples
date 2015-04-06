extern int bar(int);

void foobar(int* x, int* y) {
  for (int i = x[0]; i < x[1]; ++i) {
    for (int j = x[2]; j < x[3]; ++j) {
      y[i - j] = x[i + j] - bar(x[j]);
    }
  }
}

