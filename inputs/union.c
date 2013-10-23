typedef union {
  double dnum;
  int inum;
  float* fptr;
} my_union;


/*int foo(void* p) {*/
  /*my_union mp = *((my_union*)p);*/
  /*return mp.inum;*/
/*}*/

int bar(my_union* mu) {
  return mu[4].inum;
}
