#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include <string>
#include <vector>

using namespace llvm;

class AnalyzeLoops : public FunctionPass {
public:
  AnalyzeLoops()
      : FunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<LoopInfo>();
  }

  virtual bool runOnFunction(Function &F) {
    LoopInfo &LI = getAnalysis<LoopInfo>();
    for (LoopInfo::iterator L = LI.begin(), LE = LI.end(); L != LE; ++L) {
      (*L)->dump();
    }
    return false;
  }

  static char ID;
};

char AnalyzeLoops::ID = 0;

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
  PassManager PM;
  PM.add(new LoopInfo());
  PM.add(new AnalyzeLoops());
  PM.run(*Mod);

  return 0;
}
