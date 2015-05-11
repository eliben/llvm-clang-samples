//------------------------------------------------------------------------------
#include <set>
#include <string>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Lex/Lexer.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {
namespace ast_matchers {

// AST matcher for names in a given set
AST_MATCHER_P(clang::NamedDecl, NameInSet, std::set<std::string>,
              nameset) {
  if (auto id = Node.getIdentifier()) {
    return nameset.count(id->getName().str()) > 0;
  }
  return false;
}

}  // namespace ast_matchers
}  // namespace clang

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Matcher Sample");

//string SourceRangeAsString(const clang::SourceRange& range,
                           //const clang::SourceManager& source_manager) {
  //return "SourceRange [begin=" +
         //ast_utils::AsString(range.getBegin().printToString(source_manager)) +
         //",  end=" +
         //ast_utils::AsString(range.getEnd().printToString(source_manager)) +
         //"]";
//}
// The visitor approach
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
 public:
  void FoundRealDecl(const NamedDecl *D) {
    llvm::errs() << "**== found real decl: " << D->getQualifiedNameAsString()
                 << "\n";
  }

  bool VisitCallExpr(CallExpr *E) {
    llvm::errs() << "I see a CallExpr\n";
    E->dump();


    Expr *callee = E->getCallee();

    if (ImplicitCastExpr *ica = llvm::dyn_cast<ImplicitCastExpr>(callee)) {
      callee = ica->getSubExpr();
    }

    if (DeclRefExpr *dref = llvm::dyn_cast<DeclRefExpr>(callee)) {
      llvm::errs() << "declref:\n";
      dref->dump();

      NamedDecl *d = dref->getFoundDecl();
      ASTContext &Context = d->getASTContext();
      SourceManager &SM = Context.getSourceManager();

      if (dref->hasQualifier()) {
        llvm::errs() << "  has qualifier in name.\n";
        NestedNameSpecifierLoc lc = dref->getQualifierLoc();

        llvm::errs() << "    begin loc: " << lc.getBeginLoc().printToString(SM)
                     << "\n";
        llvm::errs() << "    end loc: " << lc.getEndLoc().printToString(SM)
                     << "\n";
      }

      if (UsingShadowDecl *sh = llvm::dyn_cast<UsingShadowDecl>(d)) {
        NamedDecl *td = sh->getTargetDecl();
        FoundRealDecl(td);
        //d->dump();
      } else {
        FoundRealDecl(d);
        //d->dump();
      }
    } else if (UnresolvedLookupExpr *ule = dyn_cast<UnresolvedLookupExpr>(callee)) {
      llvm::errs() << "unresolved\n";
      ASTContext* Context;
      SourceManager* SM;
      for (const auto *d : ule->decls()) {
        FoundRealDecl(d);
        Context = &d->getASTContext();
        SM = &Context->getSourceManager();
      }
      llvm::errs() << "    begin loc: " << ule->getLocStart().printToString(*SM)
                   << "\n";
      llvm::errs() << "    end loc: " << ule->getLocEnd().printToString(*SM)
                   << "\n";

      NestedNameSpecifierLoc ll = ule->getQualifierLoc();
      llvm::errs() << "    nested begin loc: "
                   << ll.getBeginLoc().printToString(*SM) << "\n";
      llvm::errs() << "    nested end loc: "
                   << ll.getEndLoc().printToString(*SM) << "\n";
    }

    return true;
  }
  //bool VisitIfStmt(IfStmt *s) {
    //if (const BinaryOperator *BinOP =
            //llvm::dyn_cast<BinaryOperator>(s->getCond())) {
      //if (BinOP->getOpcode() == BO_EQ) {
        //const Expr *LHS = BinOP->getLHS();
        //if (const ImplicitCastExpr *Cast =
                //llvm::dyn_cast<ImplicitCastExpr>(LHS)) {
          //LHS = Cast->getSubExpr();
        //}

        //if (const DeclRefExpr *DeclRef = llvm::dyn_cast<DeclRefExpr>(LHS)) {
          //if (const VarDecl *Var =
                  //llvm::dyn_cast<VarDecl>(DeclRef->getDecl())) {
            //if (Var->getType()->isPointerType()) {
              ////Var->dump();
            //}
          //}
        //}
      //}
    //}
    //return true;
  //}
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

struct StuffDumper : public MatchFinder::MatchCallback {
  virtual void run(const MatchFinder::MatchResult &Result) {
    auto *d = Result.Nodes.getNodeAs<Decl>("stuff");
    d->dump();
  }
};

struct RecordDumper : public MatchFinder::MatchCallback {
  virtual void run(const MatchFinder::MatchResult &Result) {
    auto *d = Result.Nodes.getNodeAs<TypeLoc>("stuff");
    SourceLocation locstart = d->getLocStart();
    llvm::errs() << "*\n";
    llvm::errs() << " start: " << locstart.printToString(*Result.SourceManager) << "\n";
    SourceLocation locend = d->getLocEnd();
    llvm::errs() << " end: " << locend.printToString(*Result.SourceManager) << "\n";
  }
};

clang::Token GetTokenBeforeLocation(clang::SourceLocation loc,
                                    const clang::ASTContext& ast_context) {
  clang::Token token;
  token.setKind(clang::tok::unknown);
  clang::LangOptions lang_options = ast_context.getLangOpts();
  const clang::SourceManager& source_manager = ast_context.getSourceManager();
  clang::SourceLocation file_start_loc =
      source_manager.getLocForStartOfFile(source_manager.getFileID(loc));
  loc = loc.getLocWithOffset(-1);
  while (loc != file_start_loc) {
    loc = clang::Lexer::GetBeginningOfToken(loc, source_manager, lang_options);
    if (!clang::Lexer::getRawToken(loc, token, source_manager, lang_options)) {
      if (!token.is(clang::tok::comment)) {
        break;
      }
    }
    loc = loc.getLocWithOffset(-1);
  }
  return token;
}

struct StuffExaminer : public MatchFinder::MatchCallback {
  virtual void run(const MatchFinder::MatchResult &Result) {
    auto *d = Result.Nodes.getNodeAs<CXXConstructorDecl>("stuff");
    if (d) {
      if (d->getNumCtorInitializers() > 0) {
        const CXXCtorInitializer *firstinit = *d->init_begin();
        if (!firstinit->isWritten()) {
          llvm::errs() << "firstinit not written; skipping\n";
          return;
        }
        if (firstinit->isBaseInitializer()) {
          TypeLoc basetypeloc = firstinit->getBaseClassLoc();
          llvm::errs() << "firstinit as base loc: "
                       << basetypeloc.getBeginLoc().printToString(
                              *Result.SourceManager) << "\n";
        }
        SourceLocation initloc = firstinit->getSourceLocation();
        llvm::errs() << "firstinit loc: "
                     << initloc.printToString(*Result.SourceManager) << "\n";
        SourceRange initrange = firstinit->getSourceRange();
        llvm::errs() << "firstinit range from "
                     << initrange.getBegin().printToString(
                            *Result.SourceManager) << "\n";
        llvm::errs() << "firstinit range to "
                     << initrange.getEnd().printToString(
                            *Result.SourceManager) << "\n";

        SourceLocation start = firstinit->getLParenLoc();
        llvm::errs() << "firstinit start: "
                     << start.printToString(*Result.SourceManager) << "\n";
        Token tok_id = GetTokenBeforeLocation(start, *Result.Context);
        llvm::errs() << "  tok_id: "
                     << tok_id.getLocation().printToString(
                            *Result.SourceManager) << "\n";
        Token tok_colon =
            GetTokenBeforeLocation(tok_id.getLocation(), *Result.Context);
        llvm::errs() << "  tok_colon: "
                     << tok_colon.getLocation().printToString(
                            *Result.SourceManager) << "\n";

        const CXXCtorInitializer *lastinit = *d->init_rbegin();
        SourceLocation end = lastinit->getRParenLoc();
        llvm::errs() << "lastinit end: "
                     << end.printToString(*Result.SourceManager) << "\n";
        //init->getInit()->dump();
      }
    }
  }
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  // Set up AST matcher callbacks.
  IfStmtHandler HandlerForIf;
  StuffDumper HandlerForStuff;
  StuffExaminer HandlerExaminer;

  MatchFinder Finder;
  const TypeMatcher AnyType = anything();

  // Find an 'if' expression with a '==' comparison, the left-hand-side of which
  // is a variable of pointer type.
  //Finder.addMatcher(
      //ifStmt(hasCondition(binaryOperator(
          //hasOperatorName("=="),
          //hasLHS(ignoringParenImpCasts(declRefExpr(
              //to(varDecl(hasType(pointsTo(AnyType))).bind("lhs")))))))),
      //&HandlerForIf);

  // Match a namespace named Vroom
  //Finder.addMatcher(
      //namespaceDecl(hasName("Vroom")).bind("stuff"),
      //&HandlerForStuff);

  // Match all function declarations in namespace Vroom
  //Finder.addMatcher(
      //functionDecl(hasAncestor(namespaceDecl(hasName("Vroom")))).bind("stuff"),
      //&HandlerForStuff);

  std::set<std::string> nameset{"foo2", "bar3", "kwa4"};

  // all functions with names from nameset and in namespace Vroom
  //Finder.addMatcher(functionDecl(NameInSet(nameset),
                                 //hasAncestor(namespaceDecl(hasName("Vroom"))),
                                 //unless(methodDecl()))
                        //.bind("stuff"),
                    //&HandlerForStuff);

  // All using declarations for name 'bolly'
  //Finder.addMatcher(
      //usingDecl(hasAnyUsingShadowDecl(hasName("bolly"))).bind("stuff"),
      //&HandlerForStuff);

  //Finder.addMatcher(functionDecl().bind("stuff"), &HandlerExaminer);
  //Finder.addMatcher(
      //constructorDecl(hasAnyConstructorInitializer(withInitializer(
                          //callExpr(callee(functionDecl(NameInSet(nameset)))))))
          //.bind("stuff"),
      //&HandlerForStuff);
      //
  RecordDumper HandlerForRecords;

  Finder.addMatcher(
      loc(templateSpecializationType(hasDeclaration(namedDecl(hasName("Frob::Homer")))))
          .bind("stuff"),
      &HandlerForRecords);

  //DeclarationMatcher ClassHomer = recordDecl(hasName("Homer"));
  //Finder.addMatcher(varDecl(hasType(ClassHomer)).bind("stuff"),
                    //&HandlerForRecords);

  llvm::outs() << "Running tool with RecursiveASTVisitor\n";
  Tool.run(newFrontendActionFactory<MyFrontendAction>().get());

  llvm::outs() << "Running tool with AST matchers\n";
  Tool.run(newFrontendActionFactory(&Finder).get());

  return 0;
}
