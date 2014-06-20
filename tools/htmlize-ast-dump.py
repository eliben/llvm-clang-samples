# This requires Python 3.4
from collections import namedtuple
import enum
import html
import io
import pprint
import re

class Color(enum.Enum):
    BLACK = 30
    READ = 31
    GREEN = 32
    YELLOW = 33
    BLUE = 34
    MAGENTA = 35
    CYAN = 36
    WHITE = 37


Token = namedtuple('Token', 'text style')

class Style:
    def __init__(self, color=Color.WHITE, bold=False):
        self.color = color
        self.bold = bold

    def __repr__(self):
        return 'Style<color={}, bold={}>'.format(self.color, self.bold)


ANSI_PATTERN = re.compile(b'\x1b\\[([^m]+)m')


def tokenize_line(line):
    # List of tokens accumulated while tokenizing the line.
    toks = []

    # The end pos of the last pattern match.
    last_end = 0

    # Current style
    cur_style = Style()

    def add_token(text):
        toks.append(Token(text, cur_style))

    for match in ANSI_PATTERN.finditer(line):
        preceding_text = line[last_end:match.start()]
        add_token(preceding_text)
        last_end = match.end()

        # Set the current style according to the ANSI code in the match.
        for ansi_code in (int(c) for c in match.group(1).split(b';')):
            if ansi_code == 0:
                # Reset
                cur_style = Style()
            elif ansi_code == 1:
                cur_style.bold = True
            else:
                # Color code. Obtain from Color enum. This will bomb if the
                # color code is invalid.
                cur_style.color = Color(ansi_code)

    leftover_text = line[last_end:]
    add_token(leftover_text)
    return toks


def htmlize(input):
    """HTML-ize the input text, producing output.

    input: stream / file-like object with textual AST dump.
    Returns a string with HTML-ized dump.
    """
    outstream = io.StringIO()

    while True:
        c = input.read(1)
        if len(c) < 1:
            # Input is finished, we're done.
            return outstream.getvalue()



def main():
    with open('/tmp/2', 'rb') as file:
        firstline = file.readline()
        print(firstline)
        print('----')
        toks = tokenize_line(firstline)
        pprint.pprint(toks)


if __name__ == '__main__':
    main()
