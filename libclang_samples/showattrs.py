import pprint
import sys
import clang.cindex
from clang.cindex import CursorKind

def visitor(cursor):
    children = list(cursor.get_children())

    for child in children:
        if (cursor.kind == CursorKind.FUNCTION_DECL and
            child.kind == CursorKind.CUDAGLOBAL_ATTR):
            print 'Function {0} is CUDA global!'.format(cursor.displayname)

        visitor(child)

index = clang.cindex.Index.create()
# Parse as CUDA
tu = index.parse(sys.argv[1], args=['-x', 'cuda'])

diagnostics = list(tu.diagnostics)
if len(diagnostics) > 0:
    print 'There were parse errors'
    pprint.pprint(diagnostics)
else:
    visitor(tu.cursor)
