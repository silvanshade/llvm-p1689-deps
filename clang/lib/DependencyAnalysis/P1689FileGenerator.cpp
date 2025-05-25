#include "P1689FileGenerator.h"
#include "clang/Basic/DiagnosticFrontend.h"
#include "clang/DependencyAnalysis/P1689DataFormatter.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

P1689FileGenerator::P1689FileGenerator(std::string_view OF) : OutputFile(OF) {}

void P1689FileGenerator::attachToPreprocessor(Preprocessor &PP) {
  if (auto RawOS = openOutput(PP.getDiagnostics())) {
    JSON.emplace(*RawOS, /*IndentSize=*/0);
    if (JSON.has_value()) {
      PP.addPPCallbacks(std::make_unique<P1689FilePPCallbacks>(PP, *JSON));
    }
  }
}

// void P1689FileGenerator::attachToASTReader(ASTReader &R) {
//   R.addListener(std::make_unique<P1689FileASTReaderListener>());
// }

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
                                           llvm::json::OStream &JSON)
    : PP(PP), JSON(JSON), Formatter(P1689DataFormatter(JSON)) {
  Formatter.p1689Begin();

  Formatter.specVersion();
  Formatter.specRevision();

  Formatter.rulesBegin();
  Formatter.ruleBegin();

  {
    WorkDirectory = getWorkDirectory();
    Formatter.workDirectory(WorkDirectory);
  }
}

auto P1689FilePPCallbacks::getWorkDirectory() -> std::optional<std::string> {
  std::optional<std::string> FilePath;
  if (const auto CWD = PP.getFileManager()
                           .getVirtualFileSystem()
                           .getCurrentWorkingDirectory()) {
    FilePath = *CWD;
  }
  return FilePath;
}

auto P1689FilePPCallbacks::getCompiledModulePath(const Module &M)
    -> std::optional<StringRef> {
  std::optional<StringRef> FilePath;
  const auto ASTFile = M.getASTFile();
  const auto CompiledModulePath = ASTFile->getName();
  if (!CompiledModulePath.empty()) {
    FilePath = CompiledModulePath;
  }
  return FilePath;
}

auto P1689FilePPCallbacks::getSourcePath(const Module &M)
    -> std::optional<StringRef> {
  auto &SM = PP.getSourceManager();
  std::optional<StringRef> FilePath;
  StringRef SourcePath =
      SM.getFileEntryRefForID(SM.getFileID(M.DefinitionLoc))->getName();
  if (!SourcePath.empty()) {
    FilePath = SourcePath;
  }
  return FilePath;
}

void P1689FilePPCallbacks::moduleImport(SourceLocation ImportLoc,
                                        ModuleIdPath Path,
                                        const Module *Imported) {

  if (State != State::REQUIRES) {
    State = State::REQUIRES;
    Formatter.requiresBegin();
  }

  StringRef LogicalName = Path.data()->first->getName();
  std::optional<std::string> CompiledModulePath;
  std::optional<StringRef> SourcePath;
  std::optional<bool> UniqueOnSourcePath;
  std::optional<P1689DataFormatter::LookupMethod> LookupMethod;

  // FIXME: Only import if not in preprocessor-only mode.
  if (!Imported) {
    Imported = PP.getModuleLoader().loadModule(ImportLoc, Path, Module::Hidden,
                                               /*IsInclusionDirective=*/false);
  }

  if (Imported) {
    CompiledModulePath = getCompiledModulePath(*Imported);
    SourcePath = getSourcePath(*Imported);
  }

  if (SourcePath.has_value() && WorkDirectory.has_value()) {
    SourcePath =
        std::filesystem::proximate(SourcePath->str(), WorkDirectory->c_str())
            .c_str();
  }

  Formatter.require(LogicalName, CompiledModulePath, SourcePath,
                    UniqueOnSourcePath, LookupMethod);
}

void P1689FilePPCallbacks::EndOfMainFile() {
  if (State == State::REQUIRES) {
    Formatter.requiresEnd();
    State.reset();
  }

  if (PP.isInNamedModule()) {
    const auto &SM = PP.getSourceManager();

    StringRef LogicalName = PP.getNamedModuleName();
    std::optional<std::string> CompiledModulePath;
    std::optional<StringRef> SourcePath =
        SM.getFileEntryRefForID(SM.getMainFileID())->getName();
    std::optional<bool> UniqueOnSourcePath;
    std::optional<bool> IsInterface = PP.isInNamedInterfaceUnit();

    if (Module *M = PP.getCurrentModule()) {
      auto &HS = PP.getHeaderSearchInfo();
      std::string FilePath;
      if (FilePath.empty()) {
        FilePath = HS.getPrebuiltModuleFileName(LogicalName);
      }
      if (FilePath.empty()) {
        FilePath = HS.getPrebuiltImplicitModuleFileName(M);
      }
      if (FilePath.empty()) {
        FilePath = HS.getCachedModuleFileName(M);
      }
      if (!FilePath.empty()) {
        CompiledModulePath = FilePath;
      }
    }

    if (SourcePath.has_value() && WorkDirectory.has_value()) {
      SourcePath =
          std::filesystem::proximate(SourcePath->str(), WorkDirectory->c_str())
              .c_str();
    }

    Formatter.providesBegin();
    Formatter.provide(LogicalName, CompiledModulePath, SourcePath,
                      UniqueOnSourcePath, IsInterface);
    Formatter.providesEnd();
  }

  Formatter.ruleEnd();
  Formatter.rulesEnd();
  Formatter.p1689End();
  JSON.flush();
}
