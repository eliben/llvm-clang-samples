LLVM & Clang library usage samples
==================================

This is a work in progress. Nothing to see here yet.

Branches
--------

LLVM & Clang evolve rapidly and the C++ API is not stable. This means that code
that links against LLVM & Clang as libraries in version X may very well not
compile or work in version X+1.

Therefore, this repository has branches that track LLVM versions. For example,
in branch llvm3.4 the code should compile and work when linked against the
released LLVM 3.4; The code in the ``master`` branch attempts to track the
upstream trunk, but may sometimes lag behind. Also, note that as more samples
are added I'll usually add them to ``master`` and will not backport them to
older branches.

Running tests and tools
-----------------------

Note: This is not really necessary to study the examples. The tests allow me to
make sure the examples keep working as LLVM advances.

For running the tests and auxiliary tools (and any other Python scripts in this
repo), Python 3 is required (I tested with 3.3+). Once the samples are built
with 'make', just run::

  python3 test/all_tests.py

