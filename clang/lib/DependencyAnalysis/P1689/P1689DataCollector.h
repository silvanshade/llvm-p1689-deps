#ifndef LLVM_CLANG_DEPENDENCYANALYSIS_P1689_P1689DATACOLLECTOR_H
#define LLVM_CLANG_DEPENDENCYANALYSIS_P1689_P1689DATACOLLECTOR_H

#include "clang/Basic/FileManager.h"
#include "clang/Lex/ModuleLoader.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/VirtualFileSystem.h"
#include <optional>
#include <string>

namespace clang::dependency_analysis::p1689 {

class P1689DataCollector {
  struct NamedModule {
    enum class Kind : uint8_t {
      Implementation,
      Interface,
    };
    StringRef Name;
    Kind Unit;
    const Module *Module;
  };

  Preprocessor &PP;
  FileManager &FM;
  llvm::vfs::FileSystem &FS;
  SourceManager &SM;
  ModuleLoader &ML;
  std::optional<std::string> WorkDirectory;

public:
  P1689DataCollector(Preprocessor &PP);

  void makeRelativeWorkDirectory(llvm::SmallVectorImpl<char> &FilePath);

  [[nodiscard]]
  auto loadModule(SourceLocation ImportLoc, ModuleIdPath Path,
                  const Module *Imported)
      -> std::optional<std::reference_wrapper<const Module>>;

  [[nodiscard]]
  auto getNamedModule() -> std::optional<NamedModule>;

  [[nodiscard]]
  auto getWorkDirectory() -> std::optional<StringRef>;

  [[nodiscard]]
  auto getCompiledModulePath(const Module &M)
      -> std::optional<llvm::SmallString<256>>;

  [[nodiscard]]
  auto getSourcePath(const Module &M) -> std::optional<llvm::SmallString<256>>;
};

} // namespace clang::dependency_analysis::p1689

#endif
