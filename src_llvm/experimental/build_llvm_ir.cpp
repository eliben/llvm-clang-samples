#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <memory>

using namespace llvm;


void MakeFunction(Module* Mod, std::string name) {
  LLVMContext &Context = Mod->getContext();
  std::vector<Type*> Args(3, Type::getDoubleTy(Context));
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(Context), Args, false);
  Function *F =
      Function::Create(FT, Function::ExternalLinkage, name, Mod);
  Function::arg_iterator arg_iter = F->arg_begin();
  Value *arg1 = &*(arg_iter++); arg1->setName("arg1");
  Value *arg2 = &*(arg_iter++); arg2->setName("arg2");
  Value *arg3 = &*(arg_iter++); arg3->setName("arg3");

  BasicBlock *BB = BasicBlock::Create(Context, "entry", F);

  // Create an IRBuilder pointing to the beginning of the entry block, and add
  // a couple of instructions.
  IRBuilder<> Builder(Context);
  Builder.SetInsertPoint(BB);

  Value *fa = Builder.CreateFAdd(arg1, arg2);
  Value *fb = Builder.CreateFAdd(fa, arg3);

  // Now create a new builder and point it to the beginning of 'entry'. Add
  // an instruction.
  IRBuilder<> TmpBuilder(BB, BB->begin());
  TmpBuilder.CreateFSub(arg3, arg2);

  // Add another instruction via the original builder. Note that it goes into
  // the end of BB, since Builder still points there.
  Builder.CreateRet(fb);
}

int main(int argc, char** argv) {
  LLVMContext &Context = getGlobalContext();
  std::unique_ptr<Module> Mod = make_unique<Module>("my module", Context);

  MakeFunction(Mod.get(), "foo");
  MakeFunction(Mod.get(), "bar");
  MakeFunction(Mod.get(), "foo");
  Mod->dump();
  return 0;
}
