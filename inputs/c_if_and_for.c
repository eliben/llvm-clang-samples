void foo(int* a, int *b) {
  if (a[0] > 1) {
    b[0] = 2;
  }
}

void bar(float x, float y); // just a declaration

void bang(int* a, int v) {
    for (int i = 0; i < v; ++i) {
        a[i] -= i;
    }
}