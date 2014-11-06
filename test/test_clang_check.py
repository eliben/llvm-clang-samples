import os, sys, unittest
from tools import run_exe


class TestClangCheck(unittest.TestCase):
    def test_live(self):
        # basic smoke test
        exe = os.path.join('build', 'clang-check')
        input = os.path.join('inputs', 'fact.c')
        rc, stdout = run_exe(exe, ['-ast-print', input, '--'])
        self.assertTrue(stdout.startswith(b'unsigned int'))

if __name__ == '__main__':
  unittest.main()
