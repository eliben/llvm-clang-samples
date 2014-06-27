#-------------------------------------------------------------------------------
# htmlize-ast-dump.py: Turn a Clang AST dump (-ast-dump) into cross-linked HTML.
#
# Run with --help for usage information.
#
# Note: this script requires Python 3.4
#
# Eli Bendersky (eliben@gmail.com)
# This code is in the public domain
#-------------------------------------------------------------------------------
import argparse
import enum
import html
import io
import pprint
import re
import sys

HTML_OUTPUT_TEMPLATE = r'''
<html>
<head>
    <style>
    html * {{
        background-color: black;
        white-space: nowrap;
    }}

    .my-pre {{
        line-height: 0.8;
        padding: 0px 0px;
        font-family: Consolas,monospace;
        font-size: 80%;
    }}

    a:link {{
        text-decoration: underline;
        color: inherit;
    }}

    a:visited {{
        text-decoration: underline;
        color: inherit;
    }}

    a:hover {{
        text-decoration: underline;
        color: #FFFFFF;
    }}

    a:active {{
        text-decoration: underline;
        color: #FFFFFF;
    }}

    .ansi-bold {{
        font-weight: bold;
        white-space: pre;
    }}

    .ansi-black {{
        color: #000000;
        white-space: pre;
    }}

    .ansi-red {{
        color: #d23737;
        white-space: pre;
    }}

    .ansi-green {{
        color: #17b217;
        white-space: pre;
    }}

    .ansi-yellow {{
        color: #b26717;
        white-space: pre;
    }}

    .ansi-blue {{
        color: #2727c2;
        white-space: pre;
    }}

    .ansi-magenta {{
        color: #b217b2;
        white-space: pre;
    }}

    .ansi-cyan {{
        color: #17b2b2;
        white-space: pre;
    }}

    .ansi-white {{
        color: #f2f2f2;
        white-space: pre;
    }}
    </style>

</head>
<body>
<pre class="my-pre">
{lines}
</pre>
<!-- Javascript -->
<script type="text/javascript">
{js_code}
</script>
</body>
</html>
'''

JS_CODE = r'''
    var myj = "abo";
    OnAnchorClick = function(elem_id) {
        alert(elem_id + myj);
    }
'''

SPAN_TEMPLATE = r'<span class="{klass}">{text}</span>'


class Color(enum.Enum):
    """Colors with values corresponding to the ANSI codes.
    """
    BLACK = 30
    RED = 31
    GREEN = 32
    YELLOW = 33
    BLUE = 34
    MAGENTA = 35
    CYAN = 36
    WHITE = 37


# Input is broken to tokens. A token is a piece of text with the style that
# applies to it. The text is decoded from binary to a string.
class Token:
    def __init__(self, text, style):
        self.text = text.decode('ascii')
        self.style = style

    def __repr__(self):
        return 'Token<text={}, style={}>'.format(self.text, self.style)


class Style:
    def __init__(self, color=Color.WHITE, bold=False):
        self.color = color
        self.bold = bold

    def __repr__(self):
        return 'Style<color={}, bold={}>'.format(self.color, self.bold)


ANSI_PATTERN = re.compile(rb'\x1b\[([^m]+)m')


def tokenize_line(line):
    """Produce (yield) a stream of tokens from an input line.
    """
    # The end pos of the last pattern match.
    last_end = 0

    # Current style
    cur_style = Style()

    for match in ANSI_PATTERN.finditer(line):
        preceding_text = line[last_end:match.start()]
        yield Token(preceding_text, cur_style)
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
    yield Token(leftover_text, cur_style)


# Link injections happens on HTML level - everything is a string now.
ADDR_PATTERN = re.compile(r'0x[0-9a-fA-F]+')

def inject_links(html_line_chunks):
    first_addr = True
    for i, chunk in enumerate(html_line_chunks):
        match = ADDR_PATTERN.search(chunk)
        if match:
            addr = match.group()
            anchorname = 'anchor_' + addr
            if first_addr:
                # The first address encountered in the line is the address of
                # the node the line describes. This becomes a link anchor.
                #print(tok.text[match.start():match.end()], file=sys.stderr)
                html_line_chunks[i] = (
                    chunk[:match.start()] +
                    '<a id="' + anchorname + '"></a>' +
                    '<a onclick="OnAnchorClick(\'' + addr +
                    '\');" href="#javascript:void(0)">' +
                    chunk[match.start():] + '</a>')
                first_addr = False
            else:
                # All other addresses refer to other nodes. These become links
                # to anchors.
                html_line_chunks[i] = (
                    chunk[:match.start()] +
                    '<a href="#' + anchorname + '">' +
                    chunk[match.start():match.end()] + '</a>' +
                    chunk[match.end():])


def analyze_line(tokens):
    """Analyzes the given line (a list of tokens).

    Returns the tuple: <id>, <name>, <nesting level>, [<used id>...]
    """
    assert(len(tokens) > 2)

    # The nesting level is always the first token
    nesting = tokens[1].text

    # The next non-empty token is the name
    itok = 2
    while len(tokens[itok].text) == 0: itok += 1 
    name = tokens[itok].text
    itok += 1

    # The next non-empty token is the id (it may not exist, though)
    while itok < len(tokens) and len(tokens[itok].text) == 0: itok += 1 
    id = tokens[itok].text if itok < len(tokens) else ''
    itok += 1
    
    # Gather all uses
    uses = []
    while itok < len(tokens):
        t = tokens[itok].text.strip()
        if ADDR_PATTERN.match(t):
            uses.append(t)
        itok += 1

    print(nesting, name, id, uses, file=sys.stderr)


def htmlize(input):
    """HTML-ize the input text, producing output.

    input: stream / file-like object with textual AST dump.
    Returns a string with HTML-ized dump.
    """
    html_lines = []
    for text_line in input:
        html_line_chunks = []
        tokens = list(tokenize_line(text_line))
        analyze_line(tokens)
        for tok in tokens:
            style = tok.style
            klass = 'ansi-{}'.format(style.color.name.lower())
            if style.bold:
                klass += ' ansi-bold'
            html_line_chunks.append(SPAN_TEMPLATE.format(
                    klass=klass,
                    text=html.escape(tok.text)))
        html_line_chunks.append('<br/>')
        inject_links(html_line_chunks)
        html_lines.append(''.join(html_line_chunks))
    return HTML_OUTPUT_TEMPLATE.format(lines='\n'.join(html_lines),
                                       js_code=JS_CODE)


def main():
    argparser = argparse.ArgumentParser(
        description='HTML output is emitted to stdout')
    argparser.add_argument('dump_file',
                           help='AST dump file, "-" for reading from stdin')
    args = argparser.parse_args()

    try:
        # Argh: it would be nice to use argparse's FileType to do this
        # automatically, but unfortunately it's broken for binary mode
        # (http://bugs.python.org/issue14156)
        input_stream = (open(sys.argv[1], 'rb') if args.dump_file != '-' else
                        io.BufferedReader(sys.stdin.buffer))
        print(htmlize(input_stream))
    finally:
        input_stream.close()


if __name__ == '__main__':
    main()
