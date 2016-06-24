//------------------------------------------------------------------------------
// analyze_geps LLVM sample. Demonstrates:
//
// * How to write a BasicBlockPass and iterate over instructions.
// * Infer all kinds of information about GEP instructions.
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//------------------------------------------------------------------------------
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"

#include <string>

using namespace llvm;

// Helper method for converting LLVM entities that support the print() method
// to a string.
template <typename T> static std::string ToString(const T *Obj) {
  std::string S;
  raw_string_ostream OS(S);
  Obj->print(OS);
  return OS.str();
}

class AnalyzeGEPPass : public BasicBlockPass {
public:
  AnalyzeGEPPass() : BasicBlockPass(ID) {}

  virtual bool runOnBasicBlock(BasicBlock &BB) {
    for (auto II = BB.begin(), II_e = BB.end(); II != II_e; ++II) {
      if (GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(II)) {
        outs() << "Found GEP:\n";
        GEP->dump();
        outs() << "  The type is: " << ToString(GEP->getType()) << "\n";
        outs() << "  The pointer operand is: "
               << ToString(GEP->getPointerOperand()) << "\n";
        outs() << "  Indices: ";
        for (auto Idx = GEP->idx_begin(), IdxE = GEP->idx_end(); Idx != IdxE;
             ++Idx) {
          outs() << "[" << ToString(Idx->get()) << "] ";
        }
        outs() << "\n";
      }
    }

    // Return false to signal that the basic block was not modified by this
    // pass.
    return false;
  }

  // The address of this member is used to uniquely identify the class. This is
  // used by LLVM's own RTTI mechanism.
  static char ID;
};

char AnalyzeGEPPass::ID = 0;

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
  PM.add(new AnalyzeGEPPass());
  PM.run(*Mod);

  return 0;
}
