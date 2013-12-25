#-------------------------------------------------------------------------------
# Makefile for building the code samples. Read inline comments for
# documentation.
#
# Eli Bendersky (eliben@gmail.com)
# This code is in the public domain
#-------------------------------------------------------------------------------

# The following variables will likely need to be modified, depending on where
# and how you built LLVM & Clang.
# LLVM_SRC_PATH is the path to the root of the checked out source code. This
# directory should contain the configure script, the include/ and lib/
# directories of LLVM, Clang in tools/clang/, etc.
# Alternatively, if you're building vs. a binary download of LLVM, then
# LLVM_SRC_PATH can point to the main untarred directory.
LLVM_SRC_PATH = $$HOME/llvm/llvm_svn_rw

# LLVM_BUILD_PATH is the directory in which you built LLVM - where you ran
# configure or cmake.
# For linking vs. a binary build of LLVM, point to the main untarred directory.
LLVM_BUILD_PATH = $$HOME/llvm/build/svn-make-debug
#LLVM_BUILD_PATH = $$HOME/llvm/build/svn-ninja-release

# LLVM_BIN_PATH is the directory where binaries are placed by the LLVM build
# process. It should contain the tools like opt, llc and clang. The default
# reflects a debug build with autotools (configure & make).
LLVM_BIN_PATH = $(LLVM_BUILD_PATH)/Debug+Asserts/bin
#LLVM_BIN_PATH = $(LLVM_BUILD_PATH)/bin

# It's recommended that CXX matches the compiler you used to build LLVM itself.
CXX := g++
CXXFLAGS := -fno-rtti -O0 -g

LLVM_CXXFLAGS := `$(LLVM_BIN_PATH)/llvm-config --cxxflags`
LLVM_LDFLAGS := `$(LLVM_BIN_PATH)/llvm-config --libs --ldflags` -ldl -lpthread

CLANG_INCLUDES := \
	-I$(LLVM_SRC_PATH)/tools/clang/include \
	-I$(LLVM_BUILD_PATH)/tools/clang/include

# List of Clang libraries to link. The proper -L will be provided by the
# call to llvm-config
# Note that I'm using -Wl,--{start|end}-group around the Clang libs; this is
# because there are circular dependencies that make the correct order difficult
# to specify and maintain. The linker group options make the linking somewhat
# slower, but IMHO they're still perfectly fine for tools that link with Clang.
CLANG_LIBS := \
	-Wl,--start-group \
	-lclangAST \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangFrontend \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangASTMatchers \
	-lclangRewriteCore \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangSerialization \
	-lclangTooling \
	-Wl,--end-group

# Internal paths in this project: where to find sources, and where to put
# build artifacts.
SRC_LLVM_DIR := src_llvm
SRC_CLANG_DIR := src_clang
BUILDDIR := build

all: make_builddir \
	$(BUILDDIR)/bb_toposort_sccs \
	$(BUILDDIR)/simple_bb_pass \
	$(BUILDDIR)/access_debug_metadata \
	$(BUILDDIR)/clang-check \
	$(BUILDDIR)/rewritersample

make_builddir:
	@test -d $(BUILDDIR) || mkdir $(BUILDDIR)

$(BUILDDIR)/simple_bb_pass: $(SRC_LLVM_DIR)/simple_bb_pass.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/access_debug_metadata: $(SRC_LLVM_DIR)/access_debug_metadata.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/bb_toposort_sccs: $(SRC_LLVM_DIR)/bb_toposort_sccs.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/clang-check: $(SRC_CLANG_DIR)/ClangCheck.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/rewritersample: $(SRC_CLANG_DIR)/rewritersample.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

clean:
	rm -rf $(BUILDDIR)/* *.dot test/*.pyc test/__pycache__
