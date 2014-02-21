"""Run with --numops=<N> to customize the size of the output.

Emits the function to stdout.
"""
import argparse
import random

FUNC_TEMPLATE = '''\
void foo(const int* in1, const int* in2, int* out) {{
{func_body}
}}
'''


def main():
    argparser = argparse.ArgumentParser()
    argparser.add_argument('--numops', type=int, default=20)
    args = argparser.parse_args()

    numops = args.numops
    bodylines = []

    def emit_new_temp(n):
        bodylines.append('  int temp{0};'.format(n))

    def emit_op(dest, inputs):
        line = '  {0} = '.format(dest)
        for n, inp in enumerate(inputs):
            line += inp
            if n < len(inputs) - 1:
                line += ' {0} '.format(random.choice(('+', '-', '*', '/', '%')))
        line += ';'
        bodylines.append(line)

    ntemps = 2
    emit_new_temp(0)
    emit_new_temp(1)

    for i in range(numops):
        # Decide on the destination. 50% chance it goes to out, 50% to new
        # temporary.
        if random.random() > 0.5:
            dest = 'out[{0}]'.format(random.randint(0, numops))
        else:
            emit_new_temp(ntemps)
            dest = 'temp{0}'.format(ntemps)
            ntemps += 1

        # Decide how many inputs it will have and where they come from (in1, in2
        # or temporaries)
        num_inputs = random.randint(2, 6)
        inputs = []
        for ni in range(num_inputs):
            choice = random.random()
            if choice > 0.8:
                inp = 'in1[{0}]'.format(random.randint(0, numops))
            elif choice > 0.6:
                inp = 'in2[{0}]'.format(random.randint(0, numops))
            else:
                # pick temp num at random
                inp = 'temp{0}'.format(random.randint(0, ntemps - 1))
            inputs.append(inp)
        emit_op(dest, inputs)

    print(FUNC_TEMPLATE.format(func_body='\n'.join(bodylines)))


if __name__ == '__main__':
    main()
