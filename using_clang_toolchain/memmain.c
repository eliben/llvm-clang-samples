#include <stdio.h>

void* memcpy_caller(void* dst, const void* src, size_t n);
void* memmove_caller(void* dst, const void* src, size_t n);
void* memset_caller(void* dst, int c, size_t n);

int main(int argc, char** argv) {
  const size_t N = 256;
  char buf[N];

  for (size_t i = 0; i < N; ++i) {
    buf[i] = i;
  }

  memmove_caller(&buf[4], &buf[0], 0);
  //memmove_caller(&buf[1], &buf[5], 0);

  for (size_t i = 0; i < 30; ++i) {
    printf("0x%x ", buf[i]);
  }
  printf("\n");

  return 0;
}
