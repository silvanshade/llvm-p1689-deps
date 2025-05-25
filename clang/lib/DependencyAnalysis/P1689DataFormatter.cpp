#include "clang/DependencyAnalysis/P1689DataFormatter.h"

namespace clang {
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
} // namespace clang

using namespace clang;

P1689DataFormatter::P1689DataFormatter(llvm::json::OStream &JSON)
    : JSON(JSON) {}

// TODO: Investigate skipping UTF-8 validation for performance.

void P1689DataFormatter::p1689Begin() { JSON.objectBegin(); }
void P1689DataFormatter::p1689End() { JSON.objectEnd(); }

void P1689DataFormatter::specVersion() { JSON.attribute("version", VERSION); }

void P1689DataFormatter::specRevision() {
  JSON.attribute("revision", REVISION);
}

void P1689DataFormatter::rulesBegin() {
  JSON.attributeBegin("rules");
  JSON.arrayBegin();
}
void P1689DataFormatter::rulesEnd() {
  JSON.arrayEnd();
  JSON.attributeEnd();
}
void P1689DataFormatter::ruleBegin() { JSON.objectBegin(); }
void P1689DataFormatter::ruleEnd() { JSON.objectEnd(); }

void P1689DataFormatter::workDirectory(std::optional<StringRef> FilePath) {
  if (FilePath.has_value()) {
    JSON.attribute("work-directory", *FilePath);
  }
}

void P1689DataFormatter::primaryOutput(StringRef FilePath) {
  JSON.attribute("primary-output", FilePath);
}

void P1689DataFormatter::outputsBegin() {
  JSON.attributeBegin("outputs");
  JSON.arrayBegin();
}
void P1689DataFormatter::outputsEnd() {
  JSON.arrayEnd();
  JSON.attributeEnd();
}
void P1689DataFormatter::output(StringRef FilePath) { JSON.value(FilePath); }

void P1689DataFormatter::providesBegin() {
  JSON.attributeBegin("provides");
  JSON.arrayBegin();
}
void P1689DataFormatter::providesEnd() {
  JSON.attributeEnd();
  JSON.arrayEnd();
}
void P1689DataFormatter::provide(StringRef LogicalName,
                                 std::optional<StringRef> CompiledModulePath,
                                 std::optional<StringRef> SourcePath,
                                 std::optional<bool> UniqueOnSourcePath,
                                 std::optional<bool> IsInterface) {
  JSON.objectBegin();
  JSON.attribute("logical-name", LogicalName);
  if (CompiledModulePath.has_value()) {
    JSON.attribute("compiled-module-path", *CompiledModulePath);
  }
  if (SourcePath.has_value()) {
    JSON.attribute("source-path", *SourcePath);
  }
  if (UniqueOnSourcePath.has_value()) {
    JSON.attribute("unique-on-source-path", *UniqueOnSourcePath);
  }
  if (IsInterface.has_value()) {
    JSON.attribute("is-interface", *IsInterface);
  }
  JSON.objectEnd();
}

void P1689DataFormatter::requiresBegin() {
  JSON.attributeBegin("requires");
  JSON.arrayBegin();
}
void P1689DataFormatter::requiresEnd() {
  JSON.attributeEnd();
  JSON.arrayEnd();
}
void P1689DataFormatter::require(StringRef LogicalName,
                                 std::optional<StringRef> CompiledModulePath,
                                 std::optional<StringRef> SourcePath,
                                 std::optional<bool> UniqueOnSourcePath,
                                 std::optional<LookupMethod> LookupMethod) {
  JSON.objectBegin();
  JSON.attribute("logical-name", LogicalName);
  if (CompiledModulePath.has_value()) {
    JSON.attribute("compiled-module-path", *CompiledModulePath);
  }
  if (SourcePath.has_value()) {
    JSON.attribute("source-path", *SourcePath);
  }
  if (UniqueOnSourcePath.has_value()) {
    JSON.attribute("unique-on-source-path", *UniqueOnSourcePath);
  }
  if (LookupMethod.has_value()) {
    JSON.attribute("lookup-method", *LookupMethod);
  }
  JSON.objectEnd();
}
