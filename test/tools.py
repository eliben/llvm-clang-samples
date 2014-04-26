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
    proc = subprocess.Popen(popen_cmd, stdout=subprocess.PIPE,
                                       stderr=subprocess.PIPE)
    proc_stdout, _ = proc.communicate()
    return proc.returncode, proc_stdout


def normalized_lines(text):
    return [line.strip() for line in text.splitlines()]


class SamplesTestCase(unittest.TestCase):
    def setUp(self):
        # Assumes it's run from the main directory, so it can find the
        # build and inputs directories as expected.
        self.build_dir = 'build'
        self.inputs_dir = 'inputs'

        # Check for a file that was placed by make to tell us where to find the
        # LLVM binaries.
        build_config_path = os.path.join(self.build_dir, '_build_config')
        if not os.path.exists(build_config_path):
            raise RuntimeError('Unable to find "%s"' % build_config_path)
        with open(build_config_path) as f:
            self.llvm_bin_path = f.readline().strip()

    def _compare_output(self, expected_out, actual_out, command=''):
        expected_lines = normalized_lines(expected_out)
        actual_lines = normalized_lines(actual_out)

        if expected_lines != actual_lines:
            print('\n!!!!!!!! %s\nDelta (actual vs. expected):' % command)
            delta = difflib.Differ().compare(expected_lines, actual_lines)
            print('\n'.join(delta))
            self.fail('Comparison failed. See delta above ^^^')

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
        self._compare_output(expected_out, stdout,
            command='{} {}'.format(cmd, input))

    def assertOptPluginOutput(self, plugin, optargs, input, expected_out):
        opt_path = os.path.join(self.llvm_bin_path, 'opt')
        input_path = os.path.join(self.inputs_dir, input)
        plugin = os.path.join(self.build_dir, plugin)
        optargs = ['-load', plugin, '--disable-output'] + optargs + [input_path]
        rc, stdout = run_exe(opt_path, optargs)
        stdout = stdout.decode('utf-8')
        self.assertEqual(rc, 0)
        self._compare_output(expected_out, stdout,
            command=optargs)

    def assertClangToolOutput(self, cmd, input, expected_out):
        sample_path = os.path.join(self.build_dir, cmd[0])
        input_path = os.path.join(self.inputs_dir, input)
        rc, stdout = run_exe(sample_path, cmd[1:] + [input_path] + ['--'])
        stdout = stdout.decode('utf-8')
        self.assertEqual(rc, 0)
        self._compare_output(expected_out, stdout,
            command='{} {}'.format(cmd, input))
