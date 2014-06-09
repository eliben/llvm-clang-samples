// INCOMPLETE SAMPLE - USE AT YOUR OWN PERIL
#include <sstream>
#include <string>

#include "clang/AST/AST.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

class IfStmtHandler : public MatchFinder::MatchCallback {
public:
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const IfStmt *FS = Result.Nodes.getNodeAs<clang::IfStmt>("ifStmt"))
      FS->dump();
  }
};

class FuncDefHandler : public MatchFinder::MatchCallback {
public:
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const FunctionDecl *FS = Result.Nodes.getNodeAs<clang::FunctionDecl>("funcDef"))
      FS->dump();
  }
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  MatchFinder Finder;
  IfStmtHandler HandlerForIf;
  FuncDefHandler HandlerForFuncDef;
  Finder.addMatcher(ifStmt().bind("ifStmt"), &HandlerForIf);
  Finder.addMatcher(functionDecl(isDefinition()).bind("funcDef"),
                    &HandlerForFuncDef);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}
