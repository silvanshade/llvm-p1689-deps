#include "clang/DependencyAnalysis/StructuredDependencyFile.h"
#include "P1689/P1689FileGenerator.h"
#include "clang/DependencyAnalysis/StructuredDependencyOutputOptions.h"

using namespace clang;

auto StructuredDependencyFileGenerator::Create(
    const StructuredDependencyOutputOptions &Opts)
    -> std::shared_ptr<StructuredDependencyFileGenerator> {
  switch (Opts.OutputFormat) {
  case StructuredDependencyOutputFormat::P1689r5:
    return std::make_shared<P1689FileGenerator>(Opts.OutputFile);
  }
}
