//------------------------------------------------------------------------------
// jit_orc_run LLVM sample. Demonstrates:
//
// * How to create a simple JIT based on LLVM's Orc API.
//
// Expects a path to a textual IR file and a function name to invoke.
// The function should have the signature: double foo(double, double)
// It's invoked with foo(2.0, 3.0) and its output is printed out.
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//------------------------------------------------------------------------------
#include <memory>

#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetSelect.h"

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
  typedef orc::ObjectLinkingLayer<> ObjLayerT;
  typedef orc::IRCompileLayer<ObjLayerT> CompileLayerT;
  typedef CompileLayerT::ModuleSetHandleT ModuleHandleT;

  SimpleOrcJIT()
      : TM(EngineBuilder().selectTarget()), DL(TM->createDataLayout()),
        CompileLayer(ObjectLayer, orc::SimpleCompiler(*TM)) {}

  // A simple SymbolResolver that doesn't support linking by always returning
  // nullptr.
  struct NoLinkingResolver : public RuntimeDyld::SymbolResolver {
    RuntimeDyld::SymbolInfo findSymbol(const std::string &Name) {
      return nullptr;
    }
    RuntimeDyld::SymbolInfo findSymbolInLogicalDylib(const std::string &Name) {
      return nullptr;
    }
  };

  // Add a module to the JIT.
  ModuleHandleT addModule(std::unique_ptr<Module> M) {
    std::vector<std::unique_ptr<Module>> MS;
    MS.push_back(std::move(M));
    auto H = CompileLayer.addModuleSet(std::move(MS),
                                       make_unique<SectionMemoryManager>(),
                                       make_unique<NoLinkingResolver>());
    ModuleHandles.push_back(H);
    return H;
  }

  // Remove a module from the JIT.
  void removeModule(ModuleHandleT H) {
    ModuleHandles.erase(
        std::find(ModuleHandles.begin(), ModuleHandles.end(), H));
    CompileLayer.removeModuleSet(H);
  }

  // Get the runtime address of the compiled symbol whose name is given.
  orc::JITSymbol findSymbol(const std::string Name) {
    std::string MangledName;
    {
      raw_string_ostream MangledNameStream(MangledName);
      Mangler::getNameWithPrefix(MangledNameStream, Name, DL);
    }

    for (auto H : make_range(ModuleHandles.rbegin(), ModuleHandles.rend())) {
      if (auto Sym = CompileLayer.findSymbolIn(H, MangledName, true)) {
        return Sym;
      }
    }

    return nullptr;
  }

private:
  std::unique_ptr<TargetMachine> TM;
  const DataLayout DL;
  ObjLayerT ObjectLayer;
  CompileLayerT CompileLayer;
  std::vector<ModuleHandleT> ModuleHandles;
};

// Signature of the function we expect.
typedef double (*FooTy)(double, double);

int main(int argc, char **argv) {
  if (argc < 3) {
    errs() << "Usage: " << argv[0] << " <function name> <IR file>\n";
    return 1;
  }

  // Parse the input LLVM IR file into a module.
  SMDiagnostic Err;
  LLVMContext Context;
  std::unique_ptr<Module> Mod(parseIRFile(argv[2], Err, Context));
  if (!Mod) {
    Err.print(argv[0], errs());
    return 1;
  }

  // This is required to initialize the MC layer for our (native) target.
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();

  SimpleOrcJIT JIT;
  JIT.addModule(std::move(Mod));

  orc::JITSymbol FooSym = JIT.findSymbol(argv[1]);
  if (!FooSym) {
    errs() << "Unable to find symbol " << argv[1] << " in module\n";
    return 1;
  }

  FooTy FooPtr = reinterpret_cast<FooTy>(FooSym.getAddress());
  outs() << argv[1] << "(2.0, 3.0) = " << FooPtr(2.0, 3.0) << "\n";

  return 0;
}
