#include <stdlib.h>

int* foo() {
#ifdef FOO
  return 0;
#else
  return NULL;
#endif
}
