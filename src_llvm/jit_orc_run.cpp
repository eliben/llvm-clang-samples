// Experimental sample
// Sample using ORC JIT: Work in progress
#include <memory>

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetSelect.h"

using namespace llvm;

class SimpleOrcJIT {
public:
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

  ModuleHandleT addModule(std::unique_ptr<Module> M) {
    std::vector<std::unique_ptr<Module>> MS;
    MS.push_back(std::move(M));
    auto H = CompileLayer.addModuleSet(std::move(MS),
                                       make_unique<SectionMemoryManager>(),
                                       make_unique<NoLinkingResolver>());

    ModuleHandles.push_back(H);
    return H;
  }

  void removeModule(ModuleHandleT H) {
    ModuleHandles.erase(
        std::find(ModuleHandles.begin(), ModuleHandles.end(), H));
    CompileLayer.removeModuleSet(H);
  }

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

typedef double (*FooTy)(double, double);

int main(int argc, char **argv) {
  if (argc < 2) {
    errs() << "Usage: " << argv[0] << " <IR file>\n";
    return 1;
  }

  // Parse the input LLVM IR file into a module.
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod(parseIRFile(argv[1], Err, getGlobalContext()));
  if (!Mod) {
    Err.print(argv[0], errs());
    return 1;
  }

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  SimpleOrcJIT JIT;
  JIT.addModule(std::move(Mod));

  orc::JITSymbol FooSym = JIT.findSymbol("foo");
  if (!FooSym) {
    errs() << "Unable to find symbol 'foo' in module";
    return 1;
  }

  FooTy FooPtr = reinterpret_cast<FooTy>(FooSym.getAddress());
  outs() << "foo(2.0, 3.0) = " << FooPtr(2.0, 3.0) << "\n";

  return 0;
}
