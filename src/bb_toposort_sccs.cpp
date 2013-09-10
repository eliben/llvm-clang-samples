//------------------------------------------------------------------------------
// bb_toposort_sccs LLVM sample. Demonstrates:
//
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//------------------------------------------------------------------------------
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"

using namespace llvm;


template <typename InfoT>
class EdgeInfo {
public:
  EdgeInfo() {}

private:
  typedef SmallDenseMap<const BasicBlock *, InfoT, 4> BBToInfoMap;
  typedef DenseMap<const BasicBlock *, BBToInfoMap *> EdgeInfoMap;

  EdgeInfoMap Map;
};

class AnalyzeBBGraph : public FunctionPass {
public:
  AnalyzeBBGraph() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) {
    const BasicBlock &EntryBB = F.getEntryBlock();
    EntryBB.dump();
    return false;
  }

  // The address of this member is used to uniquely identify the class. This is
  // used by the LLVM's own RTTI mechanism.
  static char ID;
};

char AnalyzeBBGraph::ID = 0;


int main(int argc, char **argv) {
  if (argc < 2) {
    errs() << "Usage: " << argv[0] << " <IR file>\n";
    return 1;
  }

  // Parse the input LLVM IR file into a module.
  SMDiagnostic Err;
  Module *Mod = ParseIRFile(argv[1], Err, getGlobalContext());
  if (!Mod) {
    Err.print(argv[0], errs());
    return 1;
  }

  // Create a pass manager and fill it with the passes we want to run.
  PassManager PM;
  PM.add(new AnalyzeBBGraph());
  PM.run(*Mod);

  return 0;
}
