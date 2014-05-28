import pprint
import sys
import clang.cindex

def visitor(cursor):
  print 'visiting', cursor.kind
  if cursor.kind == clang.cindex.CursorKind.CALL_EXPR:
    print 'Found %s [line=%s, col=%s]' % (
            cursor.displayname, cursor.location.line, cursor.location.column)

  children = list(cursor.get_children())
  print '  has {0} children'.format(len(children))
  for child in children:
    visitor(child)

index = clang.cindex.Index.create()
# Parse as C++
tu = index.parse(sys.argv[1], args=['-x', 'c++'])

diagnostics = list(tu.diagnostics)
if len(diagnostics) > 0:
    print 'There were parse errors'
    pprint.pprint(diagnostics)
else:
    visitor(tu.cursor)
