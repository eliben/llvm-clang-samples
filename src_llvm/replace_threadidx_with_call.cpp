// INCOMPLETE SAMPLE

//------------------------------------------------------------------------------
// replace_threadidx_with_call LLVM sample. Demonstrates:
//
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//------------------------------------------------------------------------------
#include "llvm/IR/Constants.h"
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
#include <string>
#include <vector>

using namespace llvm;

class ReplaceThreadIdxRefs : public FunctionPass {
public:
  ReplaceThreadIdxRefs(const Function *Tidx)
      : TidxRef(Tidx), FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) {
    bool Modified = false;
    for (Function::iterator I = F.begin(); I != F.end(); I++) {
      BasicBlock *BB = I;

      for (BasicBlock::iterator BI = BB->begin(), BE = BB->end();
           BI != BE; BI++) {
        if (LoadInst *Load = dyn_cast<LoadInst>(BI)) {
          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(Load->getOperand(0))) {
            // TODO: detect access to threadIdx at 0 here, and replace by
            // call to TidxRef
            Load->dump();
          }
        }
      }
    }

    return Modified;
  }

  const Function *TidxRef;

  // The address of this member is used to uniquely identify the class. This is
  // used by LLVM's own RTTI mechanism.
  static char ID;
};

char ReplaceThreadIdxRefs::ID = 0;


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

  FunctionType *TidxTy =
      FunctionType::get(Type::getInt32Ty(Mod->getContext()), false);
  Function *Tidx = Function::Create(TidxTy, GlobalValue::InternalLinkage,
      "_tidx", Mod);
  Mod->getFunction("_tidx")->dump();

  // Create a pass manager and fill it with the passes we want to run.
  PassManager PM;
  PM.add(new ReplaceThreadIdxRefs(Tidx));
  PM.run(*Mod);

  return 0;
}
