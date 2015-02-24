#-------------------------------------------------------------------------------
# htmlize-ast-dump.py: Turn a Clang AST dump (-ast-dump) into cross-linked HTML.
#
# Run with --help for usage information.
#
# Note: this script requires Python 3.4; earlier versions of Python 3 should
# work if you install the enum34 module.
#
# Eli Bendersky (eliben@gmail.com)
# This code is in the public domain
#-------------------------------------------------------------------------------
import argparse
import enum
import html
import io
import json
import pprint
import re
import sys

HTML_OUTPUT_TEMPLATE = r'''
<html>
<head>
    <style>
    .main_area, .nav_area {{
        position: absolute;
        left: 0;
        right: 0;
    }}

    .main_area {{
        top: 0;
        height: 75%;
        overflow: scroll;
        background-color: black;
        white-space: nowrap;
        padding-left: 10px;
    }}

    .nav_area {{
        bottom: 0;
        height: 25%;
        overflow: scroll;
        background-color: #131313;
    }}

    #nav_title {{
        margin-left: auto;
        margin-right: auto;
        width: 200px;
        font-weight: bold;
        color: white;
        font-size: 140%;
    }}

    #nav_contents {{
        font-family: Consolas,monospace;
        font-size: 80%;
        color: #AAAAAA;
        padding: 10px;
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
<div class="main_area">
    <pre class="my-pre">{lines}
    </pre>
</div>
<div class="nav_area">
    <div id="nav_contents">[Click on node address for cross-reference]</div>
</div>
<!-- Javascript -->
<script type="text/javascript">
    var nav_data = {nav_data};
{js_code}
</script>
</body>
</html>
'''

JS_CODE = r'''
    MakeAnchorLink = function(addr) {
        anchorname = 'anchor_' + addr
        return '<a href="#' + anchorname + '">' + addr + '</a>'
    }

    OnAnchorClick = function(elem_id) {
        var nav_entry = nav_data[elem_id];

        var contents = '';
        contents += nav_entry['name'] + ' ' + nav_entry['id'];
        contents += '<ul>\n';

        parent_id = nav_entry['parent'];

        if (parent_id === null) {
            contents += '<li>Parent: none</li>\n';
        } else {
            parent_name = nav_data[parent_id]['name']
            contents += '<li>Parent: ' + parent_name + ' ' +
                        MakeAnchorLink(parent_id) + '</li>\n';
        }

        contents += '<li>Children:'

        if (nav_entry['children'].length == 0) {
            contents += 'none</li>'
        } else {
            contents += '\n<ul>\n'
            for (var i = 0; i < nav_entry['children'].length; i++) {
                child_id = nav_entry['children'][i];
                child_name = nav_data[child_id]['name'];
                contents += '<li>' + child_name + ' ' +
                            MakeAnchorLink(child_id) + '</li>\n';
            }
            contents += '</ul>\n'
        }

        contents += '<li>Users:'

        if (nav_entry['users'].length == 0) {
            contents += 'none</li>'
        } else {
            contents += '\n<ul>\n'
            for (var i = 0; i < nav_entry['users'].length; i++) {
                user_id = nav_entry['users'][i];
                user_name = nav_data[user_id]['name'];
                contents += '<li>' + user_name + ' ' +
                            MakeAnchorLink(user_id) + '</li>\n';
            }
            contents += '</ul>\n'
        }

        document.getElementById('nav_contents').innerHTML = contents;
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


def make_anchor_link(addr, link_text):
    anchorname = 'anchor_' + addr
    return '<a href="#' + anchorname + '">' + link_text + '</a>'


def make_anchor_target(addr):
    anchorname = 'anchor_' + addr
    return '<a id="' + anchorname + '"></a>'


def inject_links(html_line_chunks):
    first_addr = True
    for i, chunk in enumerate(html_line_chunks):
        match = ADDR_PATTERN.search(chunk)
        if match:
            addr = match.group()
            if first_addr:
                # The first address encountered in the line is the address of
                # the node the line describes. This becomes a link anchor.
                #print(tok.text[match.start():match.end()], file=sys.stderr)
                html_line_chunks[i] = (
                    chunk[:match.start()] +
                    make_anchor_target(addr) +
                    '<a onclick="OnAnchorClick(\'' + addr +
                    '\');" href="#javascript:void(0)">' +
                    chunk[match.start():] + '</a>')
                first_addr = False
            else:
                # All other addresses refer to other nodes. These become links
                # to anchors.
                html_line_chunks[i] = (
                    chunk[:match.start()] +
                    make_anchor_link(addr, chunk[match.start():match.end()]) +
                    chunk[match.end():])


def analyze_line(tokens):
    """Analyzes the given line (a list of tokens).

    Returns the tuple: <id>, <name>, <nesting level>, [<used id>...]
    """
    assert(len(tokens) > 2)

    # The top-level TranslationUnitDecl node has no nesting
    if tokens[1].text.startswith('Translation'):
        nesting = ''
        itok = 1
    else:
        nesting = tokens[1].text
        itok = 2

    # The name is a concat of the following non-empty tokens, until something
    # that looks like the ID is encountered, or the line ends.
    name_parts = []
    while itok < len(tokens):
        t = tokens[itok].text.strip()
        if len(t) > 0:
            if ADDR_PATTERN.match(t):
                # Found an ID; bail out
                break
            else:
                # Part of the name
                name_parts.append(t)
        itok += 1
    name = ' '.join(name_parts)

    # Here itok is either past the end of the list, or it points to the ID.
    id = tokens[itok].text.strip() if itok < len(tokens) else ''
    itok += 1

    # Gather all uses
    uses = []
    while itok < len(tokens):
        t = tokens[itok].text.strip()
        if ADDR_PATTERN.match(t):
            uses.append(t)
        itok += 1

    nesting_level = len(nesting)
    return id, name, nesting_level, uses


def prepare_nav_data(line_info):
    """Given a list of tuples from analyze_line, prepares navigation data.

    Navigation data is a dictionary mapping an id to its children ids, paren id
    and user ids.

    It's important for line_info to be in the order gathered from the input. The
    order is essential for determining parent/child relationships.
    """
    # ZZZ: in the end, add 'users' fields...
    nav_data = {}
    def new_data_entry(line_entry):
        """Create a new entry with empty parent and child info."""
        nonlocal nav_data
        id, name, nesting_level, uselist = line_entry
        nav_data[id] = {'id': id, 'name': name,
                        'uses': uselist, 'users': [],
                        'nesting_level': nesting_level,
                        'parent': None, 'children': []}
        return nav_data[id]

    # Keep a stack of parents. The topmost parent on the stack is the one
    # collecting the current children, and their parent ID is mapped to it. The
    # stack is popped when the nesting level decreases (popped until the topmost
    # parent has a lower nesting level). Every entry is eventually pushed onto
    # the stack because it may have children.
    assert len(line_info) > 0
    assert line_info[0][2] == 0, "Expect top-level entry at nesting level 0"

    # Initialize the parent stack to the first entry
    parent_stack = [new_data_entry(line_info[0])]

    for line_entry in line_info[1:]:
        data_entry = new_data_entry(line_entry)

        # Pop the stack until the topmost entry is a suitable parent for this
        # one.
        while parent_stack[-1]['nesting_level'] >= data_entry['nesting_level']:
            # Note: no entry except the toplevel has nesting 0, so this will
            # always terminate with at most 1 entry remaining on the stack.
            parent_stack.pop()

        # Now parent_stack[-1] is the parent of this entry. Update the entries
        # accordingly.
        data_entry['parent'] = parent_stack[-1]['id']
        parent_stack[-1]['children'].append(data_entry['id'])

        # At this point, we push the current entry onto the stack.
        parent_stack.append(data_entry)

    # Finally, add 'users' fields to all entries. This is an inversion of 'uses'
    for id, entry in nav_data.items():
        for used_id in entry['uses']:
            if used_id in nav_data:
                nav_data[used_id]['users'].append(id)

    return nav_data


def htmlize(input):
    """HTML-ize the input text, producing output.

    input: stream / file-like object with textual AST dump.
    Returns a string with HTML-ized dump.
    """
    html_lines = []

    # collected list of line analysis info
    line_info = []

    for text_line in input:
        html_line_chunks = []
        tokens = list(tokenize_line(text_line))
        line_info.append(analyze_line(tokens))
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

    nav_data = prepare_nav_data(line_info)

    return HTML_OUTPUT_TEMPLATE.format(lines='\n'.join(html_lines),
                                       nav_data=json.dumps(nav_data),
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
        #tokens = list(tokenize_line(l) for l in input_stream)
        #print(list(tokens[0]))
    finally:
        input_stream.close()


if __name__ == '__main__':
    main()
