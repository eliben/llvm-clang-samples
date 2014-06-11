// Due to the special attributes, this file should be analyzed by Clang in CUDA
// mode. For example, dump the AST with:
//
// $ clang-check inputs/kernel-signatures.c -ast-dump -- -xcuda
//

__attribute__((global)) void ker1(float* farr, double* darr, int n) {
}

__attribute__((device)) void notakernel(int n, float* f1) {
}

typedef int* intptr;

__attribute__((global)) void ker2(unsigned* uarr, intptr iarr) {
}

void foobar() {
}


template <typename T>
void bazinga(T* k) {
}

int booga, chuppa;

struct St {
  int b;

  void joe(int ii, float ff) {
  	bazinga(&ii);
  	bazinga(&ff);
  }
};


