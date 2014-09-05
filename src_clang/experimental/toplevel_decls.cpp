#include <sstream>
#include <string>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R) {}

  // This gets called *while the source is being parsed* - the full AST does not
  // exist yet.
  bool HandleTopLevelDecl(DeclGroupRef DR) override {
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
      // llvm::errs() << "This is a toplevel decl!\n";
      //(*b)->dump();
    }
    return true;
  }

  // This gets called only when the full TU is completely parsed.
  void HandleTranslationUnit(ASTContext &Context) {
    llvm::errs() << "********* The whole TU *************\n";
    Context.getTranslationUnitDecl()->dump();

    llvm::errs() << "****** going over the decls stored in it:\n";
    for (auto *D : Context.getTranslationUnitDecl()->decls()) {
      llvm::errs() << "Decl in the TU:\n";
      D->dump();
      llvm::errs() << "Its start location is: '"
                   << D->getLocStart().printToString(Context.getSourceManager())
                   << "'\n";
    }
  }
};

class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}
  void EndSourceFileAction() override {
    SourceManager &SM = TheRewriter.getSourceMgr();
    llvm::errs() << "** EndSourceFileAction for: "
                 << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";

    // Now emit the rewritten buffer.
    // TheRewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    SourceManager &SM = CI.getSourceManager();
    llvm::errs() << "** Creating AST consumer for: " << file << "\n";
    llvm::errs() << "  Main file ID: "
                 << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";
    SM.PrintStats();
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return llvm::make_unique<MyASTConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
