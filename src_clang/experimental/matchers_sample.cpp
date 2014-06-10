// INCOMPLETE SAMPLE - USE AT YOUR OWN PERIL
#include <sstream>
#include <string>

#include "clang/AST/AST.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
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

class IfStmtHandler : public MatchFinder::MatchCallback {
public:
  IfStmtHandler(Replacements *Replace) : Replace(Replace) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const IfStmt *FS = Result.Nodes.getNodeAs<clang::IfStmt>("ifStmt"))
      FS->dump();
  }
private:
  Replacements *Replace;
};

class FuncDefHandler : public MatchFinder::MatchCallback {
public:
  FuncDefHandler(Replacements *Replace) : Replace(Replace) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const FunctionDecl *FS = Result.Nodes.getNodeAs<clang::FunctionDecl>("funcDef"))
      FS->dump();
  }
private:
  Replacements *Replace;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  RefactoringTool Tool(op.getCompilations(), op.getSourcePathList());

  IfStmtHandler HandlerForIf(&Tool.getReplacements());
  FuncDefHandler HandlerForFuncDef(&Tool.getReplacements());

  MatchFinder Finder;
  Finder.addMatcher(ifStmt().bind("ifStmt"), &HandlerForIf);
  Finder.addMatcher(functionDecl(isDefinition()).bind("funcDef"),
                    &HandlerForFuncDef);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}
