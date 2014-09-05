//------------------------------------------------------------------------------
// hello_pass LLVM sample. Demonstrates:
//
// * Creating a "plugin" pass loaded dynamically by opt.
// * Implementing getAnalysisUsage to notify the pass manager about possible
//   interactions with other passes.
//
// Once the .so is built, it can be loaded by opt. For example:
//
// $ opt -load build/hello_pass.so -hello-funcs <ir-file> > /dev/null
//
// Taken from the LLVM distribution with some modifications. LLVM's license
// applies.
//------------------------------------------------------------------------------
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// HelloFuncs - emit the names of functions encountered.
struct HelloFuncs : public FunctionPass {
  HelloFuncs() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) {
    outs() << "Hello: ";
    outs().write_escaped(F.getName()) << '\n';
    return false;
  }

  // We don't modify the program, so we preserve all analyses.
  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
  }

  // The address of this member is used to uniquely identify the class. This is
  // used by LLVM's own RTTI mechanism.
  static char ID;
};

char HelloFuncs::ID = 0;

// Register our pass with the pass manager in opt. For more information, see:
// http://llvm.org/docs/WritingAnLLVMPass.html
static RegisterPass<HelloFuncs>
    X("hello-funcs", "Hello World Pass (with getAnalysisUsage implemented)");
