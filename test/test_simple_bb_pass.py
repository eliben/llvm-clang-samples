import sys, unittest
from tools import SamplesTestCase


OUTPUT_FOR_FACT = '''\
i32 size 32 bits
i32 size 32 bits
i32 size 32 bits
i32 size 32 bits
i32 size 32 bits
i32 size 32 bits
'''

OUTPUT_FOR_TYPES = '''\
%struct.mystruct = type { double, i8, i32, [10 x i8] } size 256 bits
'''

PROG = 'simple_bb_pass'

class TestSimpleBBPass(SamplesTestCase):
    def test_on_fact(self):
        self.assertSampleOutput([PROG], 'fact.ll', OUTPUT_FOR_FACT)

    def test_on_types(self):
        self.assertSampleOutput([PROG], 'types.ll', OUTPUT_FOR_TYPES)

if __name__ == '__main__':
  unittest.main()
