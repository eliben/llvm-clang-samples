#-------------------------------------------------------------------------------
# Eli Bendersky (eliben@gmail.com)
# This code is in the public domain
#-------------------------------------------------------------------------------

# The following variables will likely need to be modified, depending on where
# and how you built LLVM & Clang.

# LLVM_BUILD_PATH is the directory in which you built LLVM - where you ran
# configure or cmake.
# For linking vs. a binary build of LLVM, point to the main untarred directory.
LLVM_BUILD_PATH = $$HOME/llvm/build/svn-make-debug
#LLVM_BUILD_PATH := $$HOME/llvm/build/gh-ninja-debug
#LLVM_BUILD_PATH := $$HOME/llvm/build/svn-ninja-release

# LLVM_BIN_PATH is the directory where binaries are placed by the LLVM build
# process. It should contain the tools like opt, llc and clang. The default
# reflects a debug build with autotools (configure & make).
LLVM_BIN_PATH = $(LLVM_BUILD_PATH)/Debug+Asserts/bin
#LLVM_BIN_PATH := $(LLVM_BUILD_PATH)/bin

CLANG    := $(LLVM_BIN_PATH)/clang
CLANGXX  := $(LLVM_BIN_PATH)/clang++
LLVMLINK := $(LLVM_BIN_PATH)/llvm-link
LLC      := $(LLVM_BIN_PATH)/llc
OPT 		 := $(LLVM_BIN_PATH)/opt
LD       := $(LLVM_BIN_PATH)/clang++

CFLAGS    := -std=c99 -Wall
CXXFLAGS  := -Wall
CXX11FLAG := -std=c++11

define colorecho
	@tput setaf 6
	@echo $1
	@tput sgr0
endef

BUILDDIR := build

all: make_builddir \
	$(BUILDDIR)/simple \
	$(BUILDDIR)/exceptions1 \
	$(BUILDDIR)/simple-from-bitcode \
	$(BUILDDIR)/cxx11-range-sample

make_builddir:
	@test -d $(BUILDDIR) || mkdir $(BUILDDIR)

$(BUILDDIR)/simple: simple.cpp
	$(CLANGXX) $(CXXFLAGS) $^ -o $@

$(BUILDDIR)/exceptions1: exceptions1.cpp
	$(CLANGXX) $(CXXFLAGS) $^ -o $@

# The following rules split the Clang+LLVM compilation process to discrete
# parts, generating an intermediate LLVM bitcode file and performing the backend
# compilation separately with llc.
$(BUILDDIR)/simple-from-bitcode.bc: simple.cpp
	$(call colorecho,"Compiling C++ to LLVM bitcode")
	$(CLANGXX) $(CXXFLAGS) $^ -o $@ -c -emit-llvm

$(BUILDDIR)/simple-from-bitcode.o: $(BUILDDIR)/simple-from-bitcode.bc
	$(call colorecho,"Emitting native object file from LLVM bitcode")
	$(LLC) $^ -o $@ -filetype=obj

$(BUILDDIR)/simple-from-bitcode: $(BUILDDIR)/simple-from-bitcode.o
	$(call colorecho,"Linking with" $(LD))
	$(LD) $^ -o $@

# The following rules compile a library "memcaller" to bitcode, as well as a
# "main" object that invokes it. It then uses llvm-link to link the two bitcodes
# into one and proceeds with llc to compile them to native code.
$(BUILDDIR)/memmain.bc: memmain.c
	$(CLANG) $(CFLAGS) $^ -o $@ -c -emit-llvm

$(BUILDDIR)/memcaller.bc: memcaller.c
	$(CLANG) $(CFLAGS) $^ -o $@ -c -emit-llvm

$(BUILDDIR)/memmain_exe.bc: $(BUILDDIR)/memmain.bc $(BUILDDIR)/memcaller.bc
	$(LLVMLINK) -o=$@ $^

$(BUILDDIR)/memmain_exe: $(BUILDDIR)/memmain_exe.bc
	$(LLC) $^ -o $(BUILDDIR)/memmain_exe.o -filetype=obj
	$(LD) $(BUILDDIR)/memmain_exe.o -o $@

$(BUILDDIR)/cxx11-range-sample: cxx11-range-sample.cpp
	$(CLANGXX) $(CXXFLAGS) $(CXX11FLAG) $^ -o $@

# Not adding this one to 'all' because it generates warnings
$(BUILDDIR)/warn_in_macro: warn_in_macro.cpp
	$(CLANGXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(BUILDDIR)/* *.dot test/*.pyc test/__pycache__
