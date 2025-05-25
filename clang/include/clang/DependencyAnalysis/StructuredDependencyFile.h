#ifndef LLVM_CLANG_FRONTEND_STRUCTUREDDEPENDENCYFILE_H
#define LLVM_CLANG_FRONTEND_STRUCTUREDDEPENDENCYFILE_H

#include "clang/DependencyAnalysis/DependencyCollector.h"

namespace clang {

class StructuredDependencyOutputOptions;

class StructuredDependencyFileGenerator : public DependencyCollector {
public:
  [[nodiscard]]
  static auto Create(const StructuredDependencyOutputOptions &Opts)
      -> std::shared_ptr<StructuredDependencyFileGenerator>;
};

} // namespace clang

#endif
