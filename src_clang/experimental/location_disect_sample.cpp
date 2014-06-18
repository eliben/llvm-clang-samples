// -- -x cuda -include inputs/cuda_support_header.h
#include <string>

#include "clang/AST/AST.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Lex/Lexer.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

class CallExprHandler : public MatchFinder::MatchCallback {
public:
  CallExprHandler(Replacements *Replace) : Replace(Replace) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const CallExpr *Call =
            Result.Nodes.getNodeAs<clang::CallExpr>("callExpr")) {
      if (const CUDAKernelCallExpr *KernelCall =
              llvm::dyn_cast<CUDAKernelCallExpr>(Call)) {
        KernelCall->dump();
        auto *callee = KernelCall->getCallee();
        llvm::errs() << "@@@@ Callee:\n";
        callee->dump();
        llvm::errs() << "  Location:\n";
        // Note: a SourceLocation points to a token
        callee->getLocStart().dump(*(Result.SourceManager));
        llvm::errs() << "  token length at location: "
                     << Lexer::MeasureTokenLength(
                            callee->getLocStart(), *(Result.SourceManager),
                            Result.Context->getLangOpts()) << "\n";
      }
    }
  }

private:
  Replacements *Replace;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  RefactoringTool Tool(op.getCompilations(), op.getSourcePathList());

  // Set up AST matcher callbacks.
  CallExprHandler HandlerForCall(&Tool.getReplacements());

  MatchFinder Finder;
  Finder.addMatcher(callExpr().bind("callExpr"), &HandlerForCall);
  if (int Result = Tool.run(newFrontendActionFactory(&Finder).get())) {
    return Result;
  }

  return 0;
}
