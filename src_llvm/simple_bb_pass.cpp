//------------------------------------------------------------------------------
// simple_bb_pass LLVM sample. Demonstrates:
//
// * How to write a BasicBlockPass
// * How to iterate over instructions in a basic block and detect instructions
//   of a certain type.
// * How to query for a type's size using the module's data layout.
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//------------------------------------------------------------------------------
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"

using namespace llvm;

class AllocaSizeDetect : public BasicBlockPass {
public:
  AllocaSizeDetect() : BasicBlockPass(ID) {}

  virtual bool runOnBasicBlock(BasicBlock &BB) {
    const DataLayout &DL = BB.getModule()->getDataLayout();
    for (BasicBlock::iterator II = BB.begin(), II_e = BB.end(); II != II_e;
         ++II) {
      // Iterate over each instruction in the BasicBlock. If the instruction
      // is an alloca, dump its type and query the type's size.
      if (AllocaInst *Alloca = dyn_cast<AllocaInst>(II)) {
        Type *AllocType = Alloca->getAllocatedType();
        AllocType->print(outs());
        outs() << " size " << DL.getTypeSizeInBits(AllocType) << " bits\n";
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

char AllocaSizeDetect::ID = 0;

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
  PM.add(new AllocaSizeDetect());
  PM.run(*Mod);

  return 0;
}
