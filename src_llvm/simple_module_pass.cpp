//------------------------------------------------------------------------------
// simple_module_pass LLVM sample. Demonstrates:
//
// * How to write a ModulePass
// * How to iterate over the global values in a module, and access their names
//   and types.
// * How to produce a string representation of a LLVM type.
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//------------------------------------------------------------------------------
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"

using namespace llvm;

// Helper method for converting the name of a LLVM type to a string
static std::string LLVMTypeAsString(const Type *T) {
  std::string TypeName;
  raw_string_ostream N(TypeName);
  T->print(N);
  return N.str();
}

class GVNames : public ModulePass {
public:
  GVNames() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M) {
    for (Module::const_global_iterator GI = M.global_begin(),
                                       GE = M.global_end();
         GI != GE; ++GI) {
      outs() << "Found global named \"" << GI->getName()
             << "\": type = " << LLVMTypeAsString(GI->getType()) << "\n";
    }
    return false;
  }

  // The address of this member is used to uniquely identify the class. This is
  // used by LLVM's own RTTI mechanism.
  static char ID;
};

char GVNames::ID = 0;

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

  // Create a pass manager and fill it with the passes we want to run.
  legacy::PassManager PM;
  PM.add(new GVNames());
  PM.run(*Mod);

  return 0;
}
