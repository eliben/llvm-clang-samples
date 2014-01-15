typedef struct {
  unsigned x, y, z;
} uint3;

const extern uint3 threadIdx;


int foo(int a, int b) {
  if (threadIdx.y > 5) {
    return a;
  } else {
    return b;
  }
}

