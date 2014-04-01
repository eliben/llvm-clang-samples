typedef struct {
  unsigned x, y, z;
} uint3;

volatile extern uint3 vglob;

unsigned foo(unsigned a, unsigned b, unsigned c) {
  uint3 temp = {a, b, c};
  vglob = temp;
  return temp.x + temp.y + temp.z;
}

