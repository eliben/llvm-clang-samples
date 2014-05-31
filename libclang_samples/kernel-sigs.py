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
            print '>> param "{0}" with type "{1}" [{2}, canonical = {3}]'.format(
                    c.spelling, c.type.spelling, c.type.kind.name,
                    c.type.get_canonical().spelling)


index = clang.cindex.Index.create()
# Parse as CUDA
tu = index.parse(sys.argv[1], args=['-x', 'cuda'])

diagnostics = list(tu.diagnostics)
if len(diagnostics) > 0:
    print 'There were parse errors'
    pprint.pprint(diagnostics)
else:
    for c in tu.cursor.walk_preorder():
        if c.kind == CursorKind.FUNCTION_DECL:
            handle_function_decl(c)
