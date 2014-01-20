import sys, unittest
from tools import SamplesTestCase

OUTPUT_HELLO_FUNCS = '''\
Hello: mult
Hello: fact
'''

class TestHelloPassPlugin(SamplesTestCase):
    def test_hello_on_fact(self):
        self.assertOptPluginOutput('hello_pass.so', ['-hello-funcs'], 'fact.ll',
            OUTPUT_HELLO_FUNCS)

if __name__ == '__main__':
    unittest.main()
