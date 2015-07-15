#include <string.h>

void* memcpy_impl(void* dst, const void* src, size_t n) {
  unsigned char* d = dst;
  const unsigned char* s = src;
  for (size_t i = 0; i < n; ++i) {
    d[i] = s[i];
  }
  return dst;
}

void* memmove_impl(void* dst, const void* src, size_t n) {
  unsigned char* d = dst;
  const unsigned char* s = src;
  if (s < d) {
    while (n--) {
      d[n] = s[n];
    }
  } else {
    for (size_t i = 0; i < n; ++i) {
      d[i] = s[i];
    }
  }
  return dst;
}

void* memcpy_caller(void* dst, const void* src, size_t n) {
  return memcpy(dst, src, n);
  //return memcpy_impl(dst, src, n);
}

void* memcpy_caller_casting(int* dst, const int* src, size_t n) {
  return memcpy(dst, src, n);
}

void* memmove_caller(void* dst, const void* src, size_t n) {
  //return memmove(dst, src, n);
  return memmove_impl(dst, src, n);
}

void* memset_caller(void* dst, int c, size_t n) {
  return memset(dst, c, n);
}
