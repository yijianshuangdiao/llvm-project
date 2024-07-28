//===--- DeprecatedFunctionCheck.cpp - clang-tidy----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "DeprecatedFunctionCheck.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"
#include <regex>

using namespace clang::ast_matchers;

namespace clang::tidy::misc {

std::map<std::string, std::string> parseStringToMap(const std::string &str) {
  std::map<std::string, std::string> resultMap;
  std::regex kvRegex(R"(\s*'([^']*)'\s*:\s*'([^']*)'\s*)");
  std::smatch kvMatch;
  auto begin = std::sregex_iterator(str.begin(), str.end(), kvRegex);
  auto end = std::sregex_iterator();
  for (auto it = begin; it != end; ++it) {
    std::string key = it->str(1);
    std::string value = it->str(2);
    resultMap[key] = value;
  }
  return resultMap;
}

void DeprecatedFunctionCheck::registerMatchers(MatchFinder *Finder) {
  // 从.clang-tidy中读取配置
  DeprecatedFuncAndReplaceFuncInput = DeprecatedFuncAndReplaceFuncInput.substr(
      1,
      DeprecatedFuncAndReplaceFuncInput.size() - 2); // 去除字符串首尾的大括号
  DeprecatedFuncAndReplaceFunc =
      parseStringToMap(DeprecatedFuncAndReplaceFuncInput);

  for (const auto &FuncName : DeprecatedFuncAndReplaceFunc) {
    Finder->addMatcher(callExpr(callee(functionDecl(hasName(FuncName.first))))
                           .bind(FunctionExprName),
                       this);
  }
}

DeprecatedFunctionCheck::~DeprecatedFunctionCheck() = default;

DeprecatedFunctionCheck::DeprecatedFunctionCheck(StringRef Name,
                                             ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context),
      DeprecatedFuncAndReplaceFuncInput(
          Options.get("DeprecatedFunctionInfo", "")) {}

void DeprecatedFunctionCheck::storeOptions(ClangTidyOptions::OptionMap &Opts) {

  /*
   *.clang-tidy
   * Checks:
        - misc-deprecated-function
     CheckOptions:
        - { key: misc-deprecated-function.DeprecatedFunctionInfo, value: "{'Print':'DY_LOG','PrintInfo':'DY_LOG','PrintWarning':'DY_LOG','PrintError':'DY_LOG','PrintDebug':'DY_LOG','PRINT':'DY_LOG','PrintFunc':''}" }
   */
  Options.store(Opts, "DeprecatedFunctionInfo",
                DeprecatedFuncAndReplaceFuncInput);
}

void DeprecatedFunctionCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *FunctionExpr = Result.Nodes.getNodeAs<CallExpr>(FunctionExprName);
  if (!FunctionExpr) {
    return;
  }
  const auto *CalleeDecl = FunctionExpr->getDirectCallee();
  if (!CalleeDecl) {
    return;
  }
  const auto ReplaceFunc =
      DeprecatedFuncAndReplaceFunc.find(CalleeDecl->getName().str())->second;
  if (!ReplaceFunc.empty()) {
    diag(FunctionExpr->getBeginLoc(),
         "The function '%0' method is deprecated, please use '%1' instead.",
         DiagnosticIDs::Level::Error)
        << CalleeDecl->getName() << ReplaceFunc;
  } else {
    diag(FunctionExpr->getBeginLoc(),
         "The function '%0' method is deprecated, please stop using it.",
         DiagnosticIDs::Level::Error)
        << CalleeDecl->getName();
  }
}

} // namespace clang::tidy::misc
