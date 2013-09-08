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
    
    def assertSampleOutput(self, sample, input, expected_out):
        sample_path = os.path.join(self.build_dir, sample)
        input_path = os.path.join(self.inputs_dir, input)
        rc, stdout = run_exe(sample_path, [input_path])
        self.assertEqual(rc, 0)
        self.assertEqual(stdout, expected_out)

