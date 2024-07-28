//===--- DeprecatedFunctionCheck.h - clang-tidy--------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_DEPRECATED_FUNCTION_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_DEPRECATED_FUNCTION_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::misc {

constexpr llvm::StringLiteral FunctionExprName = "FunctionExpr";

class DeprecatedFunctionCheck : public ClangTidyCheck {
public:
  DeprecatedFunctionCheck(StringRef Name, ClangTidyContext *Context);
  ~DeprecatedFunctionCheck();
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  void storeOptions(ClangTidyOptions::OptionMap &Opts) override;

private:
  std::string DeprecatedFuncAndReplaceFuncInput;
  std::map<std::string, std::string> DeprecatedFuncAndReplaceFunc;
};

} // namespace clang::tidy::misc

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_DEPRECATED_FUNCTION_H
