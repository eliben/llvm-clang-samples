CXX = g++
CXXFLAGS = -fno-rtti -O0

LLVM_SRC_PATH = $$HOME/llvm/llvm_svn_rw
LLVM_BUILD_PATH = $$HOME/llvm/build/svn-make-debug
LLVM_BIN_PATH = $(LLVM_BUILD_PATH)/Debug+Asserts/bin

LLVM_INCLUDES = -I$(LLVM_SRC_PATH)/include -I$(LLVM_BUILD_PATH)/include
LLVM_CONFIG_COMMAND = \
		`$(LLVM_BIN_PATH)/llvm-config --cxxflags --libs` \
		`$(LLVM_BIN_PATH)/llvm-config --ldflags`

# Internal paths in this project: where to find sources, and where to put
# build artifacts.
SRCDIR = src
BUILDDIR = build

all: $(BUILDDIR)/simple_bb_pass

$(BUILDDIR)/simple_bb_pass: $(SRCDIR)/simple_bb_pass.cpp
	$(CXX) $(LLVM_INCLUDES) $(CXXFLAGS) $^ $(LLVM_CONFIG_COMMAND) -o $@

clean:
	rm -rf $(BUILDDIR)/*

