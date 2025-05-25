#include "P1689FileGenerator.h"
#include "clang/Basic/DiagnosticFrontend.h"
#include "clang/DependencyAnalysis/P1689/P1689DataFormatter.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

P1689FileGenerator::P1689FileGenerator(std::string_view OF) : OutputFile(OF) {}

void P1689FileGenerator::flush() {
  if (OS.has_value()) {
    *OS << "\n";
    OS->flush();
    OS.reset();
  }
}

void P1689FileGenerator::attachToPreprocessor(Preprocessor &PP) {
  if (auto OS = openOutput(PP.getDiagnostics())) {
    JS.emplace(*OS, /*IndentSize=*/0);
    if (JS.has_value()) {
      PP.addPPCallbacks(std::make_unique<P1689FilePPCallbacks>(PP, *OS, *JS));
    }
  }
}

auto P1689FileGenerator::openOutput(DiagnosticsEngine &Diags)
    -> llvm::Expected<llvm::raw_ostream &> {
  if (OutputFile == "-") {
    return llvm::outs();
  }

  if (OutputFile == "/dev/null") {
    return llvm::nulls();
  }

  std::error_code EC;
  OS.emplace(OutputFile, EC, llvm::sys::fs::OF_Text);
  if (EC) {
    Diags.Report(diag::err_fe_unable_to_open_output)
        << OutputFile << EC.message();
    return llvm::errorCodeToError(EC);
  }

  return *OS;
}

P1689FilePPCallbacks::P1689FilePPCallbacks(Preprocessor &PP,
                                           llvm::raw_ostream &OS,
                                           llvm::json::OStream &JS)
    : DC(P1689DataCollector(PP)), DF(P1689DataFormatter(OS, JS)) {
  DF.p1689Begin();

  DF.specVersion();
  DF.specRevision();

  DF.rulesBegin();
  DF.ruleBegin();

  DF.workDirectory(DC.getWorkDirectory());
}

void P1689FilePPCallbacks::moduleImport(SourceLocation ImportLoc,
                                        ModuleIdPath Path,
                                        const Module *Imported) {

  DF.requiresBegin();

  StringRef LogicalName = Path.data()->first->getName();
  std::optional<llvm::SmallString<256>> CompiledModulePath;
  std::optional<llvm::SmallString<256>> SourcePath;
  std::optional<bool> UniqueOnSourcePath;
  std::optional<P1689DataFormatter::LookupMethod> LookupMethod;

  // FIXME: Only load module if we are analyzing AST.
  if (auto M = DC.loadModule(ImportLoc, Path, Imported)) {
    CompiledModulePath = DC.getCompiledModulePath(*M);
    SourcePath = DC.getSourcePath(*M);
  }

  DF.require(LogicalName, CompiledModulePath, SourcePath, UniqueOnSourcePath,
             LookupMethod);
}

void P1689FilePPCallbacks::EndOfMainFile() {
  DF.requiresEnd();

  if (const auto Named = DC.getNamedModule()) {
    StringRef LogicalName = Named->Name;
    std::optional<llvm::SmallString<256>> CompiledModulePath;
    std::optional<llvm::SmallString<256>> SourcePath;
    std::optional<bool> UniqueOnSourcePath;
    std::optional<bool> IsInterface;

    if (const auto *M = Named->Module) {
      CompiledModulePath = DC.getCompiledModulePath(*M);
      SourcePath = DC.getSourcePath(*M);
    }

    DF.providesBegin();
    DF.provide(LogicalName, CompiledModulePath, SourcePath, UniqueOnSourcePath,
               IsInterface);
    DF.providesEnd();
  }

  DF.ruleEnd();
  DF.rulesEnd();
  DF.p1689End();
}
