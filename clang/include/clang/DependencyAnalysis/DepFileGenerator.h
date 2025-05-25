#ifndef LLVM_CLANG_DEPENDENCYANALYSIS_DEPFILEGENERATOR_H
#define LLVM_CLANG_DEPENDENCYANALYSIS_DEPFILEGENERATOR_H

#include "clang/DependencyAnalysis/DepFileOutputOptions.h"
#include "clang/DependencyAnalysis/DependencyCollector.h"

namespace clang {

class DepFileOutputOptions;

/// Builds a dependency file when attached to a Preprocessor (for includes) and
/// ASTReader (for module imports), and writes it out at the end of processing
/// a source file.  Users should attach to the ast reader whenever a module is
/// loaded.
class DepFileGenerator : public DependencyCollector {
public:
  DepFileGenerator(const DepFileOutputOptions &Opts);

  void attachToPreprocessor(Preprocessor &PP) override;

  void finishedMainFile(DiagnosticsEngine &Diags) override;

  bool needSystemDependencies() final { return IncludeSystemHeaders; }

  bool sawDependency(StringRef Filename, bool FromModule, bool IsSystem,
                     bool IsModuleFile, bool IsMissing) final;

protected:
  void outputDepFile(llvm::raw_ostream &OS);

private:
  void outputDepFile(DiagnosticsEngine &Diags);

  std::string OutputFile;
  std::vector<std::string> Targets;
  bool IncludeSystemHeaders;
  bool PhonyTarget;
  bool AddMissingHeaderDeps;
  bool SeenMissingHeader;
  bool IncludeModuleFiles;
  DepFileOutputFormat OutputFormat;
  unsigned InputFileIndex;
};

} // namespace clang

#endif
