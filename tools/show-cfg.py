#-------------------------------------------------------------------------------
# Script for showing the CFG of the given LLVM IR file.
# 'dot' must be installed. If 'eog' is installed, it's popped to show the
# produced image.
#
# Eli Bendersky (eliben@gmail.com)
# This code is in the public domain
#-------------------------------------------------------------------------------
import os, re, subprocess, sys
import tempfile
import os.path

LLVMDIR = os.path.expanduser('~/llvm/build/svn-make-debug/Debug+Asserts/bin')
OPT = os.path.join(LLVMDIR, 'opt')

def show_dot(filename):
    outpng = os.path.join(tempfile.gettempdir(), filename + '.png')
    subprocess.check_call(['dot', '-Tpng', filename, '-o', outpng])
    print('Created', outpng, '... Running eog')
    subprocess.call('eog ' + outpng + ' &', shell=True)

def main():
    opt_out = subprocess.check_output(
        [OPT, '-dot-cfg-only', '-disable-output', sys.argv[1]],
        stderr=subprocess.STDOUT)  # opt prints stuff to stderr too o_O
    for line in opt_out.decode('utf-8').splitlines():
        m = re.search("Writing '(cfg\.[^\.]+\.dot)", line)
        if m:
            show_dot(m.group(1))
        else:
            print('Cannot parse line:', line)

if __name__ == '__main__':
    main()
