import sys, unittest
from tools import SamplesTestCase

OUTPUT_TWODOUBLEFOO_FOO = '''\
foo(2.0, 3.0) = 7.000000e+00
'''

OUTPUT_TWODOUBLEFOO_BAR = '''\
bar(2.0, 3.0) = 6.000000e+01
'''

PROG = 'jit_orc_run'

class TestJITOrcRun(SamplesTestCase):
    def test_foo(self):
        self.assertSampleOutput([PROG, 'foo'], 'twodoublefoo.ll',
                                OUTPUT_TWODOUBLEFOO_FOO)

    def test_bar(self):
        self.assertSampleOutput([PROG, 'bar'], 'twodoublefoo.ll',
                                OUTPUT_TWODOUBLEFOO_BAR)

if __name__ == '__main__':
  unittest.main()
