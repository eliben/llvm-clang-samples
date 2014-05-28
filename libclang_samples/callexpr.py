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

#def callexpr_visitor(node, parent, userdata):
    #if node.kind == clang.cindex.CursorKind.CALL_EXPR:
    #return 2 # means continue visiting recursively

index = clang.cindex.Index.create()
tu = index.parse(sys.argv[1])

visitor(tu.cursor)
