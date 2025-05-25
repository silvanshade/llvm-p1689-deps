#include "clang/DependencyAnalysis/P1689/P1689DataFormatter.h"
#include "llvm/Support/raw_ostream.h"

namespace clang::dependency_analysis::p1689 {
llvm::json::Value toJSON(const P1689DataFormatter::LookupMethod &LookupMethod) {
  switch (LookupMethod) {
  case P1689DataFormatter::LookupMethod::ByName:
    return "by-name";
  case P1689DataFormatter::LookupMethod::IncludeAngle:
    return "include-angle";
  case P1689DataFormatter::LookupMethod::IncludeQuote:
    return "include-quote";
  }
}
} // namespace clang::dependency_analysis::p1689

using namespace clang::dependency_analysis::p1689;

P1689DataFormatter::P1689DataFormatter(llvm::raw_ostream &OS,
                                       llvm::json::OStream &JS)
    : OS(OS), JS(JS) {}

// TODO: Investigate skipping UTF-8 validation for performance.

void P1689DataFormatter::p1689Begin() { JS.objectBegin(); }
void P1689DataFormatter::p1689End() {
  JS.objectEnd();
  OS << "\n";
  OS.flush();
}

void P1689DataFormatter::specVersion() { JS.attribute("version", VERSION); }

void P1689DataFormatter::specRevision() { JS.attribute("revision", REVISION); }

void P1689DataFormatter::rulesBegin() {
  JS.attributeBegin("rules");
  JS.arrayBegin();
}
void P1689DataFormatter::rulesEnd() {
  JS.arrayEnd();
  JS.attributeEnd();
}
void P1689DataFormatter::ruleBegin() { JS.objectBegin(); }
void P1689DataFormatter::ruleEnd() { JS.objectEnd(); }

void P1689DataFormatter::workDirectory(std::optional<StringRef> FilePath) {
  if (FilePath.has_value()) {
    JS.attribute("work-directory", *FilePath);
  }
}

void P1689DataFormatter::primaryOutput(StringRef FilePath) {
  JS.attribute("primary-output", FilePath);
}

void P1689DataFormatter::outputsBegin() {
  JS.attributeBegin("outputs");
  JS.arrayBegin();
}
void P1689DataFormatter::outputsEnd() {
  JS.arrayEnd();
  JS.attributeEnd();
}
void P1689DataFormatter::output(StringRef FilePath) { JS.value(FilePath); }

void P1689DataFormatter::providesBegin() {
  JS.attributeBegin("provides");
  JS.arrayBegin();
}
void P1689DataFormatter::providesEnd() {
  JS.attributeEnd();
  JS.arrayEnd();
}
void P1689DataFormatter::provide(StringRef LogicalName,
                                 std::optional<StringRef> CompiledModulePath,
                                 std::optional<StringRef> SourcePath,
                                 std::optional<bool> UniqueOnSourcePath,
                                 std::optional<bool> IsInterface) {
  JS.objectBegin();
  JS.attribute("logical-name", LogicalName);
  if (CompiledModulePath.has_value()) {
    JS.attribute("compiled-module-path", *CompiledModulePath);
  }
  if (SourcePath.has_value()) {
    JS.attribute("source-path", *SourcePath);
  }
  if (UniqueOnSourcePath.has_value()) {
    JS.attribute("unique-on-source-path", *UniqueOnSourcePath);
  }
  if (IsInterface.has_value()) {
    JS.attribute("is-interface", *IsInterface);
  }
  JS.objectEnd();
}

void P1689DataFormatter::requiresBegin() {
  if (State != State::REQUIRES) {
    State = State::REQUIRES;
    JS.attributeBegin("requires");
    JS.arrayBegin();
  }
}
void P1689DataFormatter::requiresEnd() {
  if (State == State::REQUIRES) {
    JS.arrayEnd();
    JS.attributeEnd();
    State.reset();
  }
}
void P1689DataFormatter::require(StringRef LogicalName,
                                 std::optional<StringRef> CompiledModulePath,
                                 std::optional<StringRef> SourcePath,
                                 std::optional<bool> UniqueOnSourcePath,
                                 std::optional<LookupMethod> LookupMethod) {
  JS.objectBegin();
  JS.attribute("logical-name", LogicalName);
  if (CompiledModulePath.has_value()) {
    JS.attribute("compiled-module-path", *CompiledModulePath);
  }
  if (SourcePath.has_value()) {
    JS.attribute("source-path", *SourcePath);
  }
  if (UniqueOnSourcePath.has_value()) {
    JS.attribute("unique-on-source-path", *UniqueOnSourcePath);
  }
  if (LookupMethod.has_value()) {
    JS.attribute("lookup-method", *LookupMethod);
  }
  JS.objectEnd();
}
