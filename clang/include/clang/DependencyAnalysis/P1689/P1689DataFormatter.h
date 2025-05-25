#ifndef LLVM_CLANG_DEPENDENCYANALYSIS_P1689DATAFORMATTER_H
#define LLVM_CLANG_DEPENDENCYANALYSIS_P1689DATAFORMATTER_H

#include "clang/Basic/LLVM.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"

namespace clang::dependency_analysis::p1689 {

/// P1689DataFormatter provides methods for writing formatted p1689r5 to an
/// llvm::json::OStream. The p1689r5 format is described at the link:
///
/// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1689r5.html
///
/// The specification allows for format versioning with a "version" and
/// "revision" field but note that the r5 in p1689r5 is revison of the wg
/// proposal, not the format itself.
///
/// The specification currently only defines the versioning combination:
///
/// { "version": 1, "revision": 0 }
///
/// Since there are not yet multiple versioning combinations we don't bother to
/// provide facilities for multiple formatters.
///
/// Future changes to the spec that include multiple versionings should be
/// handled by changing P1689DataFormatter into a factory that returns a derived
/// instance based on the requested combination.
class P1689DataFormatter {
  enum class State : uint8_t {
    REQUIRES,
  };

  llvm::raw_ostream &OS;
  llvm::json::OStream &JS;
  std::optional<State> State;

public:
  enum class LookupMethod : uint8_t {
    ByName,
    IncludeAngle,
    IncludeQuote,
  };

  static const uint8_t VERSION = 1;
  static const uint8_t REVISION = 0;

  P1689DataFormatter(llvm::raw_ostream &OS, llvm::json::OStream &JS);

  void p1689Begin();
  void p1689End();

  void specVersion();
  void specRevision();

  void rulesBegin();
  void rulesEnd();

  void ruleBegin();
  void ruleEnd();

  void workDirectory(std::optional<StringRef> FilePath);

  void primaryOutput(StringRef FilePath);

  void outputsBegin();
  void outputsEnd();
  void output(StringRef FilePath);

  void providesBegin();
  void providesEnd();
  void provide(StringRef LogicalName,
               std::optional<StringRef> CompiledModulePath,
               std::optional<StringRef> SourcePath,
               std::optional<bool> UniqueOnSourcePath,
               std::optional<bool> IsInterface);

  void requiresBegin();
  void requiresEnd();
  void require(StringRef LogicalName,
               std::optional<StringRef> CompiledModulePath,
               std::optional<StringRef> SourcePath,
               std::optional<bool> UniqueOnSourcePath,
               std::optional<LookupMethod> LookupMethod);
};

llvm::json::Value toJSON(const P1689DataFormatter::LookupMethod &LookupMethod);

} // namespace clang::dependency_analysis::p1689

#endif
