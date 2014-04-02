int foo(char* a, char* b);


int bar() {
  char c;
  foo(&c, 0);
  return c;
}
