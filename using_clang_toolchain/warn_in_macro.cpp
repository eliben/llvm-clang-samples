#define MYCHECK(a) (0 && 4)

int main(int argc, char** argv) {

  // Warning here
  while (0 && 4) {
  }

  // But no warning here, since 4 comes from a macro
  while (MYCHECK(argc)) {
  }

  return 0;
}
