#ifndef LLVM_CLANG_FRONTEND_STRUCTUREDDEPENDENCYOUTPUTOPTIONS_H
#define LLVM_CLANG_FRONTEND_STRUCTUREDDEPENDENCYOUTPUTOPTIONS_H

#include <string>

namespace clang {

enum class StructuredDependencyOutputFormat { P1689r5 };

class StructuredDependencyOutputOptions {
public:
  std::string OutputFile;

  StructuredDependencyOutputFormat OutputFormat;
};

} // namespace clang

#endif
