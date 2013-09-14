import difflib
import os
import subprocess
import unittest


def run_exe(exe_path, args):
    """ Runs the given executable as a subprocess, given the
        list of arguments. Captures its return code (rc) and stdout and
        returns a pair: rc, stdout_str
    """
    popen_cmd = [exe_path] + args
    proc = subprocess.Popen(popen_cmd, stdout=subprocess.PIPE)
    proc_stdout = proc.communicate()[0]
    return proc.returncode, proc_stdout


class SamplesTestCase(unittest.TestCase):
    def setUp(self):
        # Assumes it's run from the main directory, so it can find the
        # build and inputs directories as expected.
        self.build_dir = 'build'
        self.inputs_dir = 'inputs'

    def assertSampleOutput(self, cmd, input, expected_out):
        """ cmd: a list [sample_name, arg1, arg2...] - the name of the sample
                 to run, with additional arguments that will be added before
                 the path to the input file.
            input: name of a file from the input dir
            expected_out: expected output as a string
        """
        sample_path = os.path.join(self.build_dir, cmd[0])
        input_path = os.path.join(self.inputs_dir, input)
        rc, stdout = run_exe(sample_path, cmd[1:] + [input_path])
        stdout = stdout.decode('utf-8')
        self.assertEqual(rc, 0)
        if stdout != expected_out:
            print('\n!!!!!!!! %s\nDelta (actual vs. expected):' % (
                cmd + [input]))
            delta = difflib.Differ().compare(expected_out.splitlines(True),
                                             stdout.splitlines(True))
            print(''.join(delta))
            self.fail('Comparison failed. See delta above ^^^')


