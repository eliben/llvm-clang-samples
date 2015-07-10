#include <string.h>

void* memcpy_caller(void* dst, const void* src, size_t n) {
  return memcpy(dst, src, n);
}

void* memmove_caller(void* dst, const void* src, size_t n) {
  return memmove(dst, src, n);
}

void* memset_caller(void* dst, int c, size_t n) {
  return memset(dst, c, n);
}
