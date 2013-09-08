import sys, unittest
from tools import SamplesTestCase


OUTPUT_FOR_FACT = b'''\
i32 size 32 bits
i32 size 32 bits
i32 size 32 bits
i32 size 32 bits
i32 size 32 bits
i32 size 32 bits
'''

OUTPUT_FOR_TYPES = b'''\
%struct.mystruct* size 64 bits
%struct.mystruct = type { double, i8, i32, [10 x i8] } size 256 bits
i32 size 32 bits
'''


class Test(SamplesTestCase):
    def test_on_fact(self):
        self.assertSampleOutput('simple_bb_pass', 'fact.ll', OUTPUT_FOR_FACT)

    def test_on_types(self):
        self.assertSampleOutput('simple_bb_pass', 'types.ll', OUTPUT_FOR_TYPES)

if __name__ == '__main__':
  unittest.main()
