LLVM & Clang library usage samples
==================================

A collection of samples for using LLVM and Clang as libraries.

Branches
--------

LLVM & Clang evolve rapidly and the C++ API is not stable. This means that code
that links against LLVM & Clang as libraries in version X may very well not
compile or work in version X+1.

Therefore, this repository has branches that track LLVM versions. For example,
in branch ``llvm3.8`` the code should compile and work when linked against the
released LLVM 3.8; The code in the ``master`` branch attempts to track the
upstream trunk, but may sometimes lag behind. Also, note that as more samples
are added I'll usually add them to ``master`` and will not backport them to
older branches.

In any case, if anything doesn't compile as you'd expect it to, please open
an issue.

Last known LLVM build revision for the master branch
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The last upstream LLVM revision I've successfully built the ``master`` branch
against is r263167 (10-Mar-2016). It may build with newer revisions, or it may
not. If you know it builds successfully with a newer one, please let me know and
I'll update this note.

What is where
-------------

src_llvm/
    Source code for LLVM-based samples. These typically only require LLVM
    to compile and link.

src_clang/
    Source code for Clang-based samples. These require both LLVM and Clang.

using_clang_toolchain/
    Some samples of using Clang as a compilation toolchain for C and C++.

inputs/
    Some input files for the samples and tests.

test/
    Tests for the samples.

Building the samples
--------------------

A complete ``Makefile`` for Linux is included in the repository. Read the
documentation at its top; it explains how to configure the build to find your
local LLVM & Clang installation/build. A helper shell script named
``build_vs_released_binary.sh`` can make the build easier if you just point it
to a directory when you've untarred a binary rlease.

Note that LLVM & Clang use modern C++11 constructs and require a fairly
up-to-date compiler and standard C++ library to build.
`This blog post <http://eli.thegreenplace.net/2014/01/16/building-gcc-4-8-from-source-on-ubunu-12-04/>`_
may be useful if you don't have a modern C++ compiler on your machine.

Running tests and tools
-----------------------

Note: This is not really necessary to study the examples. The tests allow me to
make sure the examples keep working as LLVM advances.

For running the tests and auxiliary tools (and any other Python scripts in this
repo), Python 3 is required (I tested with 3.3+). Once the samples are built
with 'make', just run::

  make test

Note that it expects to find binaries from the LLVM directory with which the
samples were built and linked. Look at the ``emit_build_config`` rule in the
Makefile for more information.
