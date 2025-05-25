#ifndef LLVM_CLANG_DEPENDENCYANALYSIS_HEADERINCLUDEGENERATOR_H
#define LLVM_CLANG_DEPENDENCYANALYSIS_HEADERINCLUDEGENERATOR_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"

namespace clang {

class DepFileOutputOptions;
class Preprocessor;

/// AttachHeaderIncludeGen - Create a header include list generator, and attach
/// it to the given preprocessor.
///
/// \param DepOpts - Options controlling the output.
/// \param ShowAllHeaders - If true, show all header information instead of just
/// headers following the predefines buffer. This is useful for making sure
/// includes mentioned on the command line are also reported, but differs from
/// the default behavior used by -H.
/// \param OutputPath - If non-empty, a path to write the header include
/// information to, instead of writing to stderr.
/// \param ShowDepth - Whether to indent to show the nesting of the includes.
/// \param MSStyle - Whether to print in cl.exe /showIncludes style.
void AttachHeaderIncludeGenerator(Preprocessor &PP,
                                  const DepFileOutputOptions &DepOpts,
                                  bool ShowAllHeaders = false,
                                  StringRef OutputPath = {},
                                  bool ShowDepth = true, bool MSStyle = false);

} // namespace clang

#endif
