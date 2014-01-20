import os, sys, unittest

if not os.path.isdir('inputs') or not os.path.isdir('build'):
    print('ERROR: Please execute from the project root directory.')
    print('.....  And build the samples first.')
    sys.exit(1)
else:
    tests = unittest.TestLoader().discover('test', 'test*.py', 'test')
    result = unittest.TextTestRunner().run(tests)
    sys.exit(0 if result.wasSuccessful() else 1)
