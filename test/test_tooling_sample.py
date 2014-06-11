import os, sys, unittest
from tools import SamplesTestCase

OUTPUT = '''\
// Begin function foo returning void
void foo(int* a, int *b) {
  if (a[0] > 1) // the 'if' part
  {
    b[0] = 2;
  }
}
// End function foo

void bar(float x, float y); // just a declaration
'''

PROG = 'tooling_sample'

class TestToolingSample(SamplesTestCase):
    def test_live(self):
        self.assertClangToolOutput([PROG], 'cfunc_with_if.c', OUTPUT)

if __name__ == '__main__':
  unittest.main()
