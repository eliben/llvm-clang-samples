#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <memory>

using namespace llvm;

// A type encapsulating simple Orc JIT functionality. Loosely based on the
// KaleidoscopeJIT example in the LLVM tree. Doesn't support cross-module
// symbol resolution; this JIT is best used with just a single module.
class SimpleOrcJIT {
public:
  // This sample doesn't implement on-request or lazy compilation. It therefore
  // uses Orc's eager compilation layer directly - IRCompileLayer. It also uses
  // the basis object layer - ObjectLinkingLayer - directly.
  // Orc's SimpleCompiler is used to actually compile the module; it runs LLVM's
  // codegen and MC on the module, producing an object file in memory. No
  // IR-level optimizations are run by the JIT.
  typedef orc::RTDyldObjectLinkingLayer<> ObjLayerT;
  typedef orc::IRCompileLayer<ObjLayerT> CompileLayerT;
  typedef CompileLayerT::ModuleSetHandleT ModuleHandleT;

  SimpleOrcJIT()
      : TM(EngineBuilder().selectTarget()), DL(TM->createDataLayout()),
        CompileLayer(ObjectLayer, orc::SimpleCompiler(*TM)) {
    std::string s;
    bool b = llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr, &s);
    errs() << "$$ LoadLibraryPermanently returned " << b
           << "; error string=" << s << "\n";
  }

  TargetMachine &getTargetMachine() { return *TM; }

  template <typename T> static std::vector<T> singletonSet(T t) {
    std::vector<T> Vec;
    Vec.push_back(std::move(t));
    return Vec;
  }

  ModuleHandleT addModule(std::unique_ptr<Module> M) {
    // We need a memory manager to allocate memory and resolve symbols for this
    // new module. Create one that resolves symbols by looking back into the
    // JIT.
    auto Resolver = orc::createLambdaResolver(
        [&](const std::string &Name) {
          errs() << "$$ resolving " << Name << "\n";
          if (auto Sym = findMangledSymbol(Name))
            return Sym;
          return JITSymbol(nullptr);
        },
        [](const std::string &S) { 
          errs() << "$$ external resolving " << S << "\n";
          return nullptr;
        });
    auto H = CompileLayer.addModuleSet(singletonSet(std::move(M)),
                                       make_unique<SectionMemoryManager>(),
                                       std::move(Resolver));

    ModuleHandles.push_back(H);
    return H;
  }

  // Remove a module from the JIT.
  void removeModule(ModuleHandleT H) {
    ModuleHandles.erase(
        std::find(ModuleHandles.begin(), ModuleHandles.end(), H));
    CompileLayer.removeModuleSet(H);
  }

  JITSymbol findSymbol(const std::string Name) {
    errs() << "$$ findSymbol: " << Name << "\n";
    return findMangledSymbol(mangle(Name));
  }

  std::string mangle(const std::string &Name) {
    std::string MangledName;
    {
      raw_string_ostream MangledNameStream(MangledName);
      Mangler::getNameWithPrefix(MangledNameStream, Name, DL);
    }
    return MangledName;
  }

  JITSymbol findMangledSymbol(const std::string &Name) {
    errs() << "$$ findMangledSymbol: " << Name << "\n";
    const bool ExportedSymbolsOnly = true;

    // Search modules in reverse order: from last added to first added.
    // This is the opposite of the usual search order for dlsym, but makes more
    // sense in a REPL where we want to bind to the newest available definition.
    for (auto H : make_range(ModuleHandles.rbegin(), ModuleHandles.rend()))
      if (auto Sym = CompileLayer.findSymbolIn(H, Name, ExportedSymbolsOnly))
        return Sym;

    // If we can't find the symbol in the JIT, try looking in the host process.
    errs() << "$$ finding " << Name << " in host process\n";
    if (auto SymAddr = RTDyldMemoryManager::getSymbolAddressInProcess(Name))
      return JITSymbol(SymAddr, JITSymbolFlags::Exported);

    return nullptr;
  }

private:
  std::unique_ptr<TargetMachine> TM;
  const DataLayout DL;
  ObjLayerT ObjectLayer;
  CompileLayerT CompileLayer;
  std::vector<ModuleHandleT> ModuleHandles;
};

Function *MakeFunction(Module *Mod, std::string name, Function *printdfunc) {
  LLVMContext &Context = Mod->getContext();
  std::vector<Type *> Args(3, Type::getDoubleTy(Context));
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(Context), Args, false);
  Function *F = Function::Create(FT, Function::ExternalLinkage, name, Mod);
  Function::arg_iterator arg_iter = F->arg_begin();
  Value *arg1 = &*(arg_iter++);
  arg1->setName("arg1");
  Value *arg2 = &*(arg_iter++);
  arg2->setName("arg2");
  Value *arg3 = &*(arg_iter++);
  arg3->setName("arg3");

  BasicBlock *BB = BasicBlock::Create(Context, "entry", F);

  // Create an IRBuilder pointing to the beginning of the entry block, and add
  // a couple of instructions.
  IRBuilder<> Builder(Context);
  Builder.SetInsertPoint(BB);

  Value *fa = Builder.CreateFAdd(arg1, arg2);
  Value *fb = Builder.CreateFAdd(fa, arg3);
  Value *fc = Builder.CreateFAdd(arg1, arg2);

  Builder.CreateCall(printdfunc, {fc});

  Value *fd = Builder.CreateFAdd(fb, fc);

  Builder.CreateRet(fd);

  return F;
}

/// printd - printf that takes a double prints it as "%f\n", returning 0.
extern "C" double printd(double X) {
  fprintf(stderr, "%f\n", X);
  return 0;
}

// Signature of the function we expect.
typedef double (*FooTy)(double, double, double);

int main(int argc, char **argv) {
  LLVMContext Context;
  std::unique_ptr<Module> Mod = make_unique<Module>("my module", Context);

  printd(101.24);
  std::string funcname = "foo";

  FunctionType *FT =
      FunctionType::get(Type::getVoidTy(Mod->getContext()),
                        {Type::getDoubleTy(Mod->getContext())}, false);
  Function *printdfunc =
      Function::Create(FT, Function::ExternalLinkage, "printd", Mod.get());

  Function *Func = MakeFunction(Mod.get(), funcname, printdfunc);

  // This is required to initialize the MC layer for our (native) target.
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();

  SimpleOrcJIT JIT;
  Mod->setDataLayout(JIT.getTargetMachine().createDataLayout());
  void* s = llvm::sys::DynamicLibrary::SearchForAddressOfSymbol("printd");
  if (s != nullptr) {
    errs() << "$$ " << s << "\n";
  } else {
    errs() << "$$ SearchForAddressOfSymbol unable to find printd\n";
  }

  PassManagerBuilder Builder;
  Builder.OptLevel = 3;
  Builder.SizeLevel = 0;
  Builder.LoopVectorize = true;
  Builder.SLPVectorize = true;
  JIT.getTargetMachine().adjustPassManager(Builder);
  auto FPM = llvm::make_unique<legacy::FunctionPassManager>(Mod.get());
  Builder.populateFunctionPassManager(*FPM);
  FPM->doInitialization();

  if (verifyFunction(*Func, &errs())) {
    errs() << "Error verifying function... exiting\n";
    return 1;
  }
  FPM->run(*Func);
  Mod->dump();

  JIT.addModule(std::move(Mod));

  JITSymbol FooSym = JIT.findSymbol(funcname);
  if (!FooSym) {
    errs() << "Unable to find symbol " << funcname << " in module\n";
    return 1;
  }

  FooTy FooPtr = reinterpret_cast<FooTy>(FooSym.getAddress());
  outs() << FooPtr(2.0, 3.0, 7.7) << "\n";

  return 0;
}
