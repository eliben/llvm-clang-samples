char foo(char** ppc, char a, char b) {
  char* pc = *ppc;
  char c = pc[1];
  return c;
}
