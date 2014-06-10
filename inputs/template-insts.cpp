template <typename T, int N>
T foo(T a, T b) {
  T k = a;
  for (int i = 0; i < N; ++i) {
    k = k * a + b;
  }
  return k;
}


int intyfoo(int x, int y) {
  return foo<int, 6>(x, y);
}


float floatyfoo(float x, float y) {
  return foo<float, 128>(x, y);
}
