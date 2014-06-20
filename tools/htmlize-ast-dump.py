# This requires Python 3.4
from collections import namedtuple
import enum
import html
import io
import pprint
import re
import sys

# Template for full HTML output.
HTML_OUTPUT_TEMPLATE = r'''
<html>
<head>
    <style>
    html * {{
        background-color: black;
        white-space: nowrap;
    }}

    .ansi-bold {{
        font-weight: bold;
    }}

    .ansi-black {{
        color: #000000;
    }}

    .ansi-red {{
        color: #c22727;
    }}

    .ansi-green {{
        color: #17b217;
    }}

    .ansi-yellow {{
        color: #b26717;
    }}

    .ansi-blue {{
        color: #2727c2;
    }}

    .ansi-magenta {{
        color: #b217b2;
    }}

    .ansi-cyan {{
        color: #17b2b2;
    }}

    .ansi-white {{
        color: #f2f2f2;
    }}
    </style>
</head>
<body>
<code>
{lines}
</code>
</body>
</html>
'''

SPAN_TEMPLATE = r'<span class="{klass}">{text}</span>'


class Color(enum.Enum):
    BLACK = 30
    RED = 31
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
    html_lines = []
    for text_line in input:
        html_line_chunks = []
        for tok in tokenize_line(text_line):
            style = tok.style
            klass = 'ansi-{}'.format(style.color.name.lower())
            if style.bold:
                klass += ' ansi-bold'
            html_line_chunks.append(SPAN_TEMPLATE.format(
                    klass=klass,
                    text=html.escape(tok.text.decode('ascii'))))
        html_line_chunks.append('<br/>')
        html_lines.append(''.join(html_line_chunks))
    return HTML_OUTPUT_TEMPLATE.format(lines='\n'.join(html_lines))


def main():
    with open(sys.argv[1], 'rb') as file:
        print(htmlize(file))
        #firstline = file.readline()
        #print(firstline)
        #print('----')
        #toks = tokenize_line(firstline)
        #pprint.pprint(toks)
    #print(HTML_OUTPUT_TEMPLATE)


if __name__ == '__main__':
    main()
