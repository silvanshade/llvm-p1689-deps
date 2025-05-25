#ifndef LLVM_CLANG_DEPENDENCYANALYSIS_MODULEFILECOLLECTOR_H
#define LLVM_CLANG_DEPENDENCYANALYSIS_MODULEFILECOLLECTOR_H

#include "clang/Basic/LLVM.h"
#include "clang/DependencyAnalysis/DependencyCollector.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Serialization/ASTReader.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FileCollector.h"
#include "llvm/Support/VirtualFileSystem.h"

namespace clang {

/// Collects the dependencies for imported modules into a directory.  Users
/// should attach to the AST reader whenever a module is loaded.
class ModuleFileCollector : public DependencyCollector {
  std::string DestDir;
  bool HasErrors = false;
  llvm::StringSet<> Seen;
  llvm::vfs::YAMLVFSWriter VFSWriter;
  llvm::FileCollector::PathCanonicalizer Canonicalizer;

  std::error_code copyToRoot(StringRef Src, StringRef Dst = {});

public:
  ModuleFileCollector(std::string DestDir) : DestDir(std::move(DestDir)) {}
  ~ModuleFileCollector() override { writeFileMap(); }

  StringRef getDest() { return DestDir; }
  virtual bool insertSeen(StringRef Filename) {
    return Seen.insert(Filename).second;
  }
  virtual void addFile(StringRef Filename, StringRef FileDst = {});

  virtual void addFileMapping(StringRef VPath, StringRef RPath) {
    VFSWriter.addFileMapping(VPath, RPath);
  }

  void attachToPreprocessor(Preprocessor &PP) override;
  void attachToASTReader(ASTReader &R) override;

  virtual void writeFileMap();
  virtual bool hasErrors() { return HasErrors; }
};

} // namespace clang

#endif
