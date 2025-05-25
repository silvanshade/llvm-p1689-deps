#ifndef LLVM_CLANG_DEPENDENCYANALYSIS_P1689FILE_H
#define LLVM_CLANG_DEPENDENCYANALYSIS_P1689FILE_H

#include "clang/Basic/Diagnostic.h"
#include "clang/DependencyAnalysis/DependencyCollector.h"
#include "clang/DependencyAnalysis/P1689DataFormatter.h"
#include "clang/DependencyAnalysis/StructuredDependencyFile.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Serialization/ASTReader.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"
#include <filesystem>

namespace clang {

class P1689FileASTReaderListener;
class P1689FilePPCallbacks;

class P1689FileGenerator : public StructuredDependencyFileGenerator {
  std::string_view OutputFile;
  std::optional<llvm::raw_fd_ostream> OS;
  std::optional<llvm::json::OStream> JSON;
  std::optional<StringRef> CWD;

public:
  P1689FileGenerator(std::string_view OF);

  void attachToPreprocessor(Preprocessor &PP) override;

  // void attachToASTReader(ASTReader &R) override;

  [[nodiscard]]
  auto openOutput(DiagnosticsEngine &Diags)
      -> llvm::Expected<llvm::raw_ostream &>;
};

class P1689FilePPCallbacks : public PPCallbacks {
  enum class State : uint8_t { REQUIRES };

  Preprocessor &PP;
  llvm::json::OStream &JSON;
  P1689DataFormatter Formatter;
  std::optional<State> State;
  std::optional<std::string> WorkDirectory;

  [[nodiscard]]
  auto getWorkDirectory() -> std::optional<std::string>;

  [[nodiscard]]
  auto getCompiledModulePath(const Module &M) -> std::optional<StringRef>;

  [[nodiscard]]
  auto getSourcePath(const Module &M) -> std::optional<StringRef>;

public:
  P1689FilePPCallbacks(Preprocessor &PP, llvm::json::OStream &JsonOS);

  void moduleImport(SourceLocation ImportLoc, ModuleIdPath Path,
                    const Module *Imported) override;

  void EndOfMainFile() override;
};

} // namespace clang

#endif
