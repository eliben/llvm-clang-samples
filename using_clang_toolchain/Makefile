#-------------------------------------------------------------------------------
# Eli Bendersky (eliben@gmail.com)
# This code is in the public domain
#-------------------------------------------------------------------------------

# The following variables will likely need to be modified, depending on where
# and how you built LLVM & Clang.

# LLVM_BUILD_PATH is the directory in which you built LLVM - where you ran
# configure or cmake.
# For linking vs. a binary build of LLVM, point to the main untarred directory.
#LLVM_BUILD_PATH = $$HOME/llvm/build/svn-make-debug
LLVM_BUILD_PATH := $$HOME/llvm/build/svn-ninja-release

# LLVM_BIN_PATH is the directory where binaries are placed by the LLVM build
# process. It should contain the tools like opt, llc and clang. The default
# reflects a debug build with autotools (configure & make).
#LLVM_BIN_PATH = $(LLVM_BUILD_PATH)/Debug+Asserts/bin
LLVM_BIN_PATH := $(LLVM_BUILD_PATH)/bin

CLANGXX := $(LLVM_BIN_PATH)/clang++

CCFLAGS := -Wall

BUILDDIR := build

all: make_builddir \
			$(BUILDDIR)/simple

make_builddir:
	@test -d $(BUILDDIR) || mkdir $(BUILDDIR)

$(BUILDDIR)/simple: simple.cpp
	$(CLANGXX) $(CCFLAGS) $^ -o $@

clean:
	rm -rf $(BUILDDIR)/* *.dot test/*.pyc test/__pycache__
