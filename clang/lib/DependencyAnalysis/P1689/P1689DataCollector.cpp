#include "P1689DataCollector.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Path.h"
#include <optional>

using namespace clang;
using namespace clang::dependency_analysis::p1689;

P1689DataCollector::P1689DataCollector(Preprocessor &PP)
    : PP(PP), FM(PP.getFileManager()), FS(FM.getVirtualFileSystem()),
      SM(PP.getSourceManager()), ML(PP.getModuleLoader()) {
  if (const auto CWD = FS.getCurrentWorkingDirectory()) {
    WorkDirectory = *CWD;
  }
}

void P1689DataCollector::makeRelativeWorkDirectory(
    llvm::SmallVectorImpl<char> &FilePath) {
  if (WorkDirectory.has_value()) {
    llvm::sys::path::replace_path_prefix(FilePath,
                                         /*OldPrefix=*/*WorkDirectory,
                                         /*NewPrefix=*/"");
  }
}

auto P1689DataCollector::loadModule(SourceLocation ImportLoc, ModuleIdPath Path,
                                    const Module *Imported)
    -> std::optional<std::reference_wrapper<const Module>> {
  if (!Imported) {
    Imported = ML.loadModule(ImportLoc, Path, Module::Hidden,
                             /*IsInclusionDirective=*/false);
  }
  return *Imported;
}

auto P1689DataCollector::getNamedModule() -> std::optional<NamedModule> {
  std::optional<NamedModule> Named;
  if (PP.isInNamedModule()) {
    StringRef Name;
    NamedModule::Kind Unit;
    const Module *Module = PP.getCurrentModule();
    Name = PP.getNamedModuleName();
    if (PP.isInImplementationUnit()) {
      Unit = NamedModule::Kind::Implementation;
    } else if (PP.isInNamedInterfaceUnit()) {
      Unit = NamedModule::Kind::Interface;
    } else {
      llvm_unreachable("Named module should always correspond to a unit type");
    }
    Named = NamedModule{Name, Unit, Module};
  }
  return Named;
}

auto P1689DataCollector::getWorkDirectory() -> std::optional<StringRef> {
  return WorkDirectory;
}

auto P1689DataCollector::getCompiledModulePath(const Module &M)
    -> std::optional<llvm::SmallString<256>> {
  llvm::SmallString<256> FilePath;
  if (const auto ASTFile = M.getASTFile()) {
    FilePath = ASTFile->getName();
  }
  {
    auto &HS = PP.getHeaderSearchInfo();
    if (FilePath.empty()) {
      FilePath = HS.getPrebuiltModuleFileName(M.Name);
    }
    if (FilePath.empty()) {
      FilePath = HS.getCachedModuleFileName(&M);
    }
  }
  makeRelativeWorkDirectory(FilePath);
  return FilePath.empty() ? std::nullopt : std::optional{FilePath};
}

auto P1689DataCollector::getSourcePath(const Module &M)
    -> std::optional<llvm::SmallString<256>> {
  const auto FileID = SM.getFileID(M.DefinitionLoc);
  const auto FileRef = SM.getFileEntryRefForID(FileID);
  llvm::SmallString<256> FilePath = FileRef->getName();
  makeRelativeWorkDirectory(FilePath);
  return FilePath.empty() ? std::nullopt : std::optional{FilePath};
}
