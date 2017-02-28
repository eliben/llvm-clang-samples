//------------------------------------------------------------------------------
// replace_threadidx_with_call LLVM sample. Demonstrates:
//
// * Detecting a non-trivial instruction pattern (load instruction with a
//   specific form of getelementptr constant expression address).
// * Adding new function declarations into the module.
// * Performing an IR transformation: replacing instructions by other
//   instructions.
//
// Note: this is not a complete transformation for CUDA's special globals, but
// a much simplified example.
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//------------------------------------------------------------------------------
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Operator.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include <string>
#include <vector>

using namespace llvm;

class ReplaceThreadIdxRefs : public FunctionPass {
public:
  ReplaceThreadIdxRefs(Function *Tidx, Function *Tidy, Function *Tidz)
      : FunctionPass(ID), TidxRef(Tidx), TidyRef(Tidy), TidzRef(Tidz) {}

  virtual bool runOnFunction(Function &F) {
    bool Modified = false;
    for (Function::iterator I = F.begin(); I != F.end(); I++) {
      BasicBlock *BB = &*I;

      for (BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE;) {
        // Note: taking BI++ out of the for statement is important. Since this
        // loop may delete the instruction at *BI, this will invalidate the
        // iterator. So we make sure the iterator is incremented right from
        // the start and it already points to the next instruction. This way,
        // removing I from the basic block is harmless.
        Instruction &I = *BI++;

        // These nested conditions match a specific instruction pattern. We're
        // looking for a load whose address is a GEP constant expression.
        if (LoadInst *Load = dyn_cast<LoadInst>(&I)) {
          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(Load->getOperand(0))) {
            if (GEPOperator *GEP = dyn_cast<GEPOperator>(CE)) {
              Value *Ptr = GEP->getPointerOperand();

              // Only look for accesses to threadIdx with the expected amount of
              // GEP indices (essentially struct access to threadIdx.<member>.
              if (Ptr->getName() != "threadIdx" || GEP->getNumIndices() != 2) {
                continue;
              }

              // struct access as a GEP has two indices; read the LLVM
              // documentation on GEPs if this doesn't make sense.
              if (ConstantInt *CI = dyn_cast<ConstantInt>(GEP->getOperand(2))) {
                // Choose a function based on the index.
                uint64_t DimIndex = CI->getZExtValue();
                Function *TargetFunc;
                if (DimIndex == 0) {
                  TargetFunc = TidxRef;
                } else if (DimIndex == 1) {
                  TargetFunc = TidyRef;
                } else if (DimIndex == 2) {
                  TargetFunc = TidzRef;
                } else {
                  report_fatal_error("Invalid index for threadIdx access");
                }

                // Create a call instruction to the appropriate _tid* function
                // right before the load and replace the load by it.
                CallInst *TidFuncCall = CallInst::Create(TargetFunc, "", Load);
                TidFuncCall->takeName(Load);
                Load->replaceAllUsesWith(TidFuncCall);
                Load->eraseFromParent();
                Modified = true;
              }
            }
          }
        }
      }
    }

    return Modified;
  }

private:
  Function *TidxRef;
  Function *TidyRef;
  Function *TidzRef;

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
  LLVMContext Context;
  std::unique_ptr<Module> Mod(parseIRFile(argv[1], Err, Context));
  if (!Mod) {
    Err.print(argv[0], errs());
    return 1;
  }

  // Create a function declarations for _tidx, _tidy, _tidz
  FunctionType *TidFuncTy =
      FunctionType::get(Type::getInt32Ty(Mod->getContext()), false);
  Function *Tidx = Function::Create(TidFuncTy, GlobalValue::InternalLinkage,
                                    "_tidx", Mod.get());
  Function *Tidy = Function::Create(TidFuncTy, GlobalValue::InternalLinkage,
                                    "_tidy", Mod.get());
  Function *Tidz = Function::Create(TidFuncTy, GlobalValue::InternalLinkage,
                                    "_tidz", Mod.get());

  // Create a pass manager and fill it with the passes we want to run.
  legacy::PassManager PM;
  PM.add(new ReplaceThreadIdxRefs(Tidx, Tidy, Tidz));
  PM.run(*Mod);

  outs() << "Dumping the module after the pass has run:\n";
  Mod->dump();

  return 0;
}
