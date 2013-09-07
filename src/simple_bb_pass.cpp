#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Operator.h"
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
using namespace llvm;

class MyBBPass : public BasicBlockPass {
public:
  static char ID;

  MyBBPass() : BasicBlockPass(ID) {}

  virtual bool runOnBasicBlock(BasicBlock &BB) {
    Instruction *last = BB.getTerminator();
    Instruction *prev = last->getPrevNode();
    errs() << "Last:\n";
    last->dump();
    if (prev) {
      errs() << "  Prev:\n";
      errs() << "  ";
      prev->dump();
    }
    for (BasicBlock::iterator ii = BB.begin(), ii_e = BB.end(); ii != ii_e;
         ++ii) {
      if (GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(&*ii)) {
        // Dump the GEP instruction
        gep->dump();
        Value *firstOperand = gep->getOperand(0);
        Type *type = firstOperand->getType();

        // Figure out whether the first operand points to an array
        if (PointerType *pointerType = dyn_cast<PointerType>(type)) {
          Type *elementType = pointerType->getElementType();
          errs() << "The element type is: " << *elementType << "\n";

          if (elementType->isArrayTy()) {
            errs() << "  .. points to an array!\n";
          }
        }
      }
    }

    return false;
  }
};

char MyBBPass::ID = 0;

int main(int argc, char **argv) {
  if (argc < 2) {
    errs() << "Expected an argument - IR file name\n";
    exit(1);
  }

  std::vector<Instruction> vi;

  // outs() << "\033[31m" << "Hello from main\n";
  outs().changeColor(raw_ostream::RED) << "I would this RED\n";
  outs().resetColor();

  LLVMContext &Context = getGlobalContext();
  SMDiagnostic Err;
  Module *Mod = ParseIRFile(argv[1], Err, Context);

  if (!Mod) {
    Err.print(argv[0], errs());
    return 1;
  }

  PassManager Passes;
  MyBBPass *MP = new MyBBPass();
  Passes.add(MP);

  Passes.run(*Mod);

  // errs() << *Mod;
  return 0;
}
