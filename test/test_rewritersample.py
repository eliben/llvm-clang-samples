import sys, unittest
from tools import SamplesTestCase

OUTPUT_FACT = '''\
// Begin function mult returning unsigned int
unsigned mult(unsigned a, unsigned b) {
  unsigned i, s = 0;
  for (i = 0; i < a; ++i) {
    s += b;
  }
  return s;
}
// End function mult

// Begin function fact returning unsigned int
unsigned fact(unsigned a) {
  if (a <= 1) // the 'if' part
  {
    return 1;
  } else // the 'else' part
  {
    return a * fact(a - 1);
  }
}
// End function fact
'''


PROG = 'rewritersample'

class TestRewriterSample(SamplesTestCase):
    def test_on_fact(self):
        self.assertSampleOutput([PROG], 'fact.c', OUTPUT_FACT)

if __name__ == '__main__':
  unittest.main()


