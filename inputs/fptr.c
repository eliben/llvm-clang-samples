extern void (*v)(void*);

void foo(int* x, int *y, void (*f)(int*)) {
  v = f;
}


