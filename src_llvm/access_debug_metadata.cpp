//------------------------------------------------------------------------------
// This is currently ad-hoc
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//------------------------------------------------------------------------------
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"

using namespace llvm;

int main(int argc, char **argv) {
  if (argc < 2) {
    errs() << "Usage: " << argv[0] << " <IR file>\n";
    return 1;
  }

  // Parse the input LLVM IR file into a module.
  SMDiagnostic Err;
  LLVMContext Context;
  std::unique_ptr<Module> Mod(parseIRFile(argv[1], Err, Context));
  if (!Mod) {
    Err.print(argv[0], errs());
    return 1;
  }

  // Go over all named mdnodes in the module
  for (Module::const_named_metadata_iterator I = Mod->named_metadata_begin(),
                                             E = Mod->named_metadata_end();
       I != E; ++I) {
    outs() << "Found MDNode:\n";
    I->dump();

    for (unsigned i = 0, e = I->getNumOperands(); i != e; ++i) {
      Metadata *Op = I->getOperand(i);
      if (auto *N = dyn_cast<MDNode>(Op)) {
        outs() << "  Has MDNode operand:\n  ";
        N->dump();
      }
    }
  }

  return 0;
}
