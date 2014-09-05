import pprint
import sys
import clang.cindex

def get_cursor(source, spelling):
    """Obtain a cursor from a source object.

    This provides a convenient search mechanism to find a cursor with specific
    spelling within a source. The first argument can be either a
    TranslationUnit or Cursor instance.

    If the cursor is not found, None is returned.
    """
    # Convenience for calling on a TU.

    root_cursor = (source if isinstance(source, clang.cindex.Cursor)
                          else source.cursor)

    for cursor in root_cursor.walk_preorder():
        if cursor.spelling == spelling:
            return cursor

    return None

src = '''\
int foo(int, int);
'''

tu = clang.cindex.TranslationUnit.from_source('t.cpp',
                                              ['-x', 'c++'],
                                              unsaved_files=[('t.cpp', src)])

foo = get_cursor(tu, 'foo')

print(foo.mangled_name)
