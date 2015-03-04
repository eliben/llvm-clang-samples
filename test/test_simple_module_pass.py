import sys, unittest
from tools import SamplesTestCase


OUTPUT_FOR_GLOBALS = '''\
Found global named "gfloat": type = float*
Found global named "gppfloat": type = float***
Found global named "gint": type = i32*
'''

PROG = 'simple_module_pass'

class TestSimpleModulePass(SamplesTestCase):
    def test_on_globals(self):
        self.assertSampleOutput([PROG], 'globals.ll', OUTPUT_FOR_GLOBALS)

if __name__ == '__main__':
  unittest.main()
