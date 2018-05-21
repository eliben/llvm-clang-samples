#-------------------------------------------------------------------------------
# Sample makefile for building the code samples. Read inline comments for
# documentation.
#
# Eli Bendersky (eliben@gmail.com)
# This code is in the public domain
#-------------------------------------------------------------------------------

# The following variables will likely need to be customized, depending on where
# and how you built LLVM & Clang. They can be overridden by setting them on the
# make command line: "make VARNAME=VALUE", etc.

# LLVM_SRC_PATH is the path to the root of the checked out source code. This
# directory should contain the configure script, the include/ and lib/
# directories of LLVM, Clang in tools/clang/, etc.
#
# The most recent build from source of LLVM I did used the following cmake
# invocation:
#
# $ cmake -DCMAKE_BUILD_TYPE=Release \
#         -DLLVM_ENABLE_ASSERTIONS=ON \
#         -DCMAKE_C_FLAGS=-DLLVM_ENABLE_DUMP \
#         -DCMAKE_CXX_FLAGS=-DLLVM_ENABLE_DUMP \
#         -DLLVM_TARGETS_TO_BUILD="X86" \
#         -G Ninja
#
# Note that this is a release build with assertions enabled, and with
# LLVM_ENABLE_DUMP explicitly passed. This is required to get the LLVM IR-level
# 'dump' methods to work. With debug builds, assertions should be enabled by
# default. Also note that a fairly recent version of cmake is required; the
# latest I've been using is 3.5.2
#
# It is recommended to use a recent Clang to build LLVM itself. For example if
# you have a Clang binary release in $CLANGDIR, cmake should be run with:
#
# $ CC=$CLANGDIR/bin/clang CXX=$CLANGDIR/bin/clang++ cmake ....
#
# Alternatively, if you're building vs. a binary distribution of LLVM
# (downloaded from llvm.org), then LLVM_SRC_PATH can point to the main untarred
# directory of the binary download (the directory that has bin/, lib/, include/
# and other directories inside).
# See the build_vs_released_binary.sh script for an example.
#
#  Changes for building against the Homebrew install of llvm on OSX platforms
#  (provided by b.van straalen  Lawrence Berkeley National Lab)
#  1. >brew install llvm --with-toolchain
#  2. LLVM_SRC_PATH := /usr/local/opt/llvm
#  3. LLVM_BUILD_PATH := $(LLVM_SRC_PATH)
#  4. CXX:=$(LLVM_BUILD_PATH)/bin/clang++
#  5. PLUGIN_LDFLAGS := -shared -undefined dynamic_lookup
#  6. -Wl,--start-group \   changed to  -Wl, \
#  7  -Wl,--end-group    changed to  -Wl,

LLVM_SRC_PATH := $$HOME/llvm/llvm_svn_rw

# LLVM_BUILD_PATH is the directory in which you built LLVM - where you ran
# configure or cmake.
# For linking vs. a binary build of LLVM, point to the main untarred directory.
# LLVM_BIN_PATH is the directory where binaries are placed by the LLVM build
# process. It should contain the tools like opt, llc and clang. The default
# reflects a release build with CMake and Ninja. binary build of LLVM, point it
# to the bin/ directory.
LLVM_BUILD_PATH := $$HOME/llvm/build/svn-ninja-release
LLVM_BIN_PATH 	:= $(LLVM_BUILD_PATH)/bin

$(info -----------------------------------------------)
$(info Using LLVM_SRC_PATH = $(LLVM_SRC_PATH))
$(info Using LLVM_BUILD_PATH = $(LLVM_BUILD_PATH))
$(info Using LLVM_BIN_PATH = $(LLVM_BIN_PATH))
$(info -----------------------------------------------)

CXX := g++
CXXFLAGS := -fno-rtti -O0 -g
PLUGIN_CXXFLAGS := -fpic

LLVM_CXXFLAGS := `$(LLVM_BIN_PATH)/llvm-config --cxxflags`
LLVM_LDFLAGS := `$(LLVM_BIN_PATH)/llvm-config --ldflags --libs --system-libs`

# Plugins shouldn't link LLVM and Clang libs statically, because they are
# already linked into the main executable (opt or clang). LLVM doesn't like its
# libs to be linked more than once because it uses globals for configuration
# and plugin registration, and these trample over each other.
LLVM_LDFLAGS_NOLIBS := `$(LLVM_BIN_PATH)/llvm-config --ldflags`
PLUGIN_LDFLAGS := -shared

# These are required when compiling vs. a source distribution of Clang. For
# binary distributions llvm-config --cxxflags gives the right path.
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
	-lclangASTMatchers \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangCrossTU \
	-lclangIndex \
	-lclangSerialization \
	-lclangToolingCore \
	-lclangTooling \
	-lclangFormat \
	-Wl,--end-group

# Internal paths in this project: where to find sources, and where to put
# build artifacts.
SRC_LLVM_DIR := src_llvm
SRC_CLANG_DIR := src_clang
BUILDDIR := build

.PHONY: all
all: make_builddir \
	emit_build_config \
	$(BUILDDIR)/bb_toposort_sccs \
	$(BUILDDIR)/simple_module_pass \
	$(BUILDDIR)/simple_bb_pass \
	$(BUILDDIR)/analyze_geps \
	$(BUILDDIR)/hello_pass.so \
	$(BUILDDIR)/replace_threadidx_with_call \
	$(BUILDDIR)/access_debug_metadata \
	$(BUILDDIR)/clang-check \
	$(BUILDDIR)/rewritersample \
	$(BUILDDIR)/matchers_rewriter \
	$(BUILDDIR)/tooling_sample \
	$(BUILDDIR)/plugin_print_funcnames.so

.PHONY: test
test: emit_build_config
	python3 test/all_tests.py

.PHONY: emit_build_config
emit_build_config: make_builddir
	@echo $(LLVM_BIN_PATH) > $(BUILDDIR)/_build_config

.PHONY: make_builddir
make_builddir:
	@test -d $(BUILDDIR) || mkdir $(BUILDDIR)

$(BUILDDIR)/simple_bb_pass: $(SRC_LLVM_DIR)/simple_bb_pass.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/analyze_geps: $(SRC_LLVM_DIR)/analyze_geps.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/replace_threadidx_with_call: $(SRC_LLVM_DIR)/replace_threadidx_with_call.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/simple_module_pass: $(SRC_LLVM_DIR)/simple_module_pass.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/access_debug_metadata: $(SRC_LLVM_DIR)/access_debug_metadata.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/bb_toposort_sccs: $(SRC_LLVM_DIR)/bb_toposort_sccs.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/hello_pass.so: $(SRC_LLVM_DIR)/hello_pass.cpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) \
		$^ $(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@

$(BUILDDIR)/clang-check: $(SRC_CLANG_DIR)/ClangCheck.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/rewritersample: $(SRC_CLANG_DIR)/rewritersample.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/tooling_sample: $(SRC_CLANG_DIR)/tooling_sample.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/matchers_rewriter: $(SRC_CLANG_DIR)/matchers_rewriter.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/plugin_print_funcnames.so: $(SRC_CLANG_DIR)/plugin_print_funcnames.cpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@

# Experimental tools - use at your own peril.
#
.PHONY: experimental_tools
experimental_tools: make_builddir \
	emit_build_config \
	$(BUILDDIR)/loop_info \
	$(BUILDDIR)/build_llvm_ir \
	$(BUILDDIR)/remove-cstr-calls \
	$(BUILDDIR)/toplevel_decls \
	$(BUILDDIR)/try_matcher

$(BUILDDIR)/loop_info: $(SRC_LLVM_DIR)/experimental/loop_info.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

# build_llvm_ir needs -rdynamic so that it can dlsym symbols from its own
# binary in the JIT.
$(BUILDDIR)/build_llvm_ir: $(SRC_LLVM_DIR)/experimental/build_llvm_ir.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ -rdynamic $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/remove-cstr-calls: $(SRC_CLANG_DIR)/experimental/RemoveCStrCalls.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/toplevel_decls: $(SRC_CLANG_DIR)/experimental/toplevel_decls.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/try_matcher: $(SRC_CLANG_DIR)/experimental/try_matcher.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

.PHONY: clean format

clean:
	rm -rf $(BUILDDIR)/* *.dot test/*.pyc test/__pycache__

format:
	find . -name "*.cpp" | xargs clang-format -style=file -i
