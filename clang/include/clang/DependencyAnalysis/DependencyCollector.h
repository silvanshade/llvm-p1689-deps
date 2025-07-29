#ifndef LLVM_CLANG_DEPENDENCYANALYSIS_DEPENDENCYCOLLECTOR_H
#define LLVM_CLANG_DEPENDENCYANALYSIS_DEPENDENCYCOLLECTOR_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringSet.h"

namespace clang {

class ASTReader;
class DiagnosticsEngine;
class Preprocessor;

/// An interface for collecting the dependencies of a compilation. Users should
/// use \c attachToPreprocessor and \c attachToASTReader to get all of the
/// dependencies.
/// FIXME: Migrate DependencyGraphGen to use this interface.
class DependencyCollector {
public:
  static bool isSpecialFilename(StringRef Filename);

  virtual ~DependencyCollector();

  virtual void attachToPreprocessor(Preprocessor &PP);
  virtual void attachToASTReader(ASTReader &R);
  ArrayRef<std::string> getDependencies() const { return Dependencies; }

  /// Called when a new file is seen. Return true if \p Filename should be added
  /// to the list of dependencies.
  ///
  /// The default implementation ignores <built-in> and system files.
  virtual bool sawDependency(StringRef Filename, bool FromModule,
                             bool IsSystem, bool IsModuleFile, bool IsMissing);

  /// Called when the end of the main file is reached.
  virtual void finishedMainFile(DiagnosticsEngine &Diags) {}

  /// Return true if system files should be passed to sawDependency().
  virtual bool needSystemDependencies() { return false; }

  /// Add a dependency \p Filename if it has not been seen before and
  /// sawDependency() returns true.
  virtual void maybeAddDependency(StringRef Filename, bool FromModule,
                                  bool IsSystem, bool IsModuleFile,
                                  bool IsMissing);

protected:
  /// Return true if the filename was added to the list of dependencies, false
  /// otherwise.
  bool addDependency(StringRef Filename);

private:
  llvm::StringSet<> Seen;
  std::vector<std::string> Dependencies;
};

} // namespace clang

#endif
