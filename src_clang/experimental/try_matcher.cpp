//------------------------------------------------------------------------------
#include <string>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Matcher Sample");

// The visitor approach
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  bool VisitIfStmt(IfStmt *s) {
    if (const BinaryOperator *BinOP =
            llvm::dyn_cast<BinaryOperator>(s->getCond())) {
      if (BinOP->getOpcode() == BO_EQ) {
        const Expr *LHS = BinOP->getLHS();
        if (const ImplicitCastExpr *Cast =
                llvm::dyn_cast<ImplicitCastExpr>(LHS)) {
          LHS = Cast->getSubExpr();
        }

        if (const DeclRefExpr *DeclRef = llvm::dyn_cast<DeclRefExpr>(LHS)) {
          if (const VarDecl *Var =
                  llvm::dyn_cast<VarDecl>(DeclRef->getDecl())) {
            if (Var->getType()->isPointerType()) {
              Var->dump();
            }
          }
        }
      }
    }
    return true;
  }
};

struct MyASTConsumer : public ASTConsumer {
  bool HandleTopLevelDecl(DeclGroupRef DR) override {
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
      Visitor.TraverseDecl(*b);
    }
    return true;
  }

  MyASTVisitor Visitor;
};

struct MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    return llvm::make_unique<MyASTConsumer>();
  }
};

// The AST matcher approach
struct IfStmtHandler : public MatchFinder::MatchCallback {
  virtual void run(const MatchFinder::MatchResult &Result) {
    const VarDecl *lhs = Result.Nodes.getNodeAs<VarDecl>("lhs");
    lhs->dump();
  }
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  // Set up AST matcher callbacks.
  IfStmtHandler HandlerForIf;
  MatchFinder Finder;
  const TypeMatcher AnyType = anything();

  // Find an 'if' expression with a '==' comparison, the left-hand-side of which
  // is a variable of pointer type.
  Finder.addMatcher(
      ifStmt(hasCondition(binaryOperator(
          hasOperatorName("=="),
          hasLHS(ignoringParenImpCasts(declRefExpr(
              to(varDecl(hasType(pointsTo(AnyType))).bind("lhs")))))))),
      &HandlerForIf);

  llvm::outs() << "Running tool with RecursiveASTVisitor\n";
  Tool.run(newFrontendActionFactory<MyFrontendAction>().get());

  llvm::outs() << "Running tool with AST matchers\n";
  Tool.run(newFrontendActionFactory(&Finder).get());

  return 0;
}
