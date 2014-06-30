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

clang::Token GetTokenAfterLocation(clang::SourceLocation loc,
                                   const clang::SourceManager &source_manager) {
  clang::Token token;
  token.setKind(clang::tok::unknown);
  clang::LangOptions lang_options;
  loc = loc.getLocWithOffset(1);
  while (loc !=
         source_manager.getLocForEndOfFile(source_manager.getFileID(loc))) {
    if (!clang::Lexer::getRawToken(loc, token, source_manager, lang_options)) {
      if (!token.is(clang::tok::comment)) {
        break;
      }
      loc = clang::Lexer::getLocForEndOfToken(token.getLocation(), /*Offset=*/0,
                                              source_manager, lang_options);
    } else {
      loc = loc.getLocWithOffset(1);
    }
  }
  return token;
}

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

        const SourceManager *SM = Result.SourceManager;
        const LangOptions &LOpts = Result.Context->getLangOpts();

        // Note: a SourceLocation points to a token
        llvm::errs() << "  Callee location:\n";
        callee->getLocStart().dump(*SM);
        llvm::errs() << "  token length at location: "
                     << Lexer::MeasureTokenLength(callee->getLocStart(), *SM,
                                                  LOpts) << "\n";

        for (const auto *Arg : KernelCall->arguments()) {
          llvm::errs() << "  argument location:\n";
          llvm::errs() << "    start: ";
          Arg->getLocStart().dump(*SM);
          llvm::errs() << "\n";

          Token tokbefore = GetTokenBefore(*SM, Arg->getLocStart());
          llvm::errs() << "   token before this arg is: " << tokbefore.getName()
                       << " at location ";
          tokbefore.getLocation().dump(*SM);
          llvm::errs() << "\n";

          llvm::errs() << "    end: ";
          Arg->getLocEnd().dump(*SM);
          llvm::errs() << "\n";
        }
      } else {
        // Experiment to play with source locations.
        const SourceManager *SM = Result.SourceManager;
        SourceLocation loc_call = Call->getLocStart();
        llvm::errs() << "Call loc: " << loc_call.printToString(*SM) << "\n";
        Token t = GetTokenAfterLocation(loc_call, *SM);
        llvm::errs() << " tok: " << t.getName()
                     << " raw_id: " << t.getRawIdentifier()
                     << " loc=" << t.getLocation().printToString(*SM) << "\n";
        t = GetTokenAfterLocation(
            t.getLocation().getLocWithOffset(t.getLength()), *SM);
        llvm::errs() << " tok: " << t.getName()
                     << " raw_id: " << t.getRawIdentifier()
                     << " loc=" << t.getLocation().printToString(*SM) << "\n";
      }
    }
  }

  clang::Token GetTokenBefore(const clang::SourceManager &sources,
                              clang::SourceLocation loc) {
    clang::Token token;
    token.setKind(clang::tok::unknown);
    clang::LangOptions lang_options;
    loc = loc.getLocWithOffset(-1);
    while (loc != sources.getLocForStartOfFile(sources.getFileID(loc))) {
      loc = clang::Lexer::GetBeginningOfToken(loc, sources, lang_options);
      if (!clang::Lexer::getRawToken(loc, token, sources, lang_options)) {
        if (!token.is(clang::tok::comment)) {
          break;
        }
      }
      loc = loc.getLocWithOffset(-1);
    }
    return token;
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
