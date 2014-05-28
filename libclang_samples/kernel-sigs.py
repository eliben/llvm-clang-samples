import pprint
import sys
import clang.cindex
from clang.cindex import CursorKind


def handle_function_decl(fdecl_cursor):
    children = list(fdecl_cursor.get_children())

    # Only interested in functions that have a CUDAGLOBAL_ATTR attached.
    if not any(c.kind == CursorKind.CUDAGLOBAL_ATTR for c in children):
        return

    print fdecl_cursor.displayname

    # Look at param decls
    for c in children:
        if c.kind == CursorKind.PARM_DECL:
            print '>>', c.spelling, c.type.spelling


def visitor(cursor):
    if cursor.kind == CursorKind.FUNCTION_DECL:
        handle_function_decl(cursor)

    for child in cursor.get_children():
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
