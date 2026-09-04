# Single-pass P1689 dependency emission in the Clang frontend

Emits [P1689R5](https://wg21.link/p1689r5) C++20 module dependency information
**during compilation**, from the frontend, instead of requiring a separate
`clang-scan-deps` pass.

- **Base:** `47addd4540b4` (LLVM `release/20.x`, 2025-05-21)
- **Status:** working, work in progress. Not yet proposed upstream.
- **Diff:** [`47addd4540b4...p1689-frontend`](https://github.com/silvanshade/llvm-p1689-deps/compare/47addd4540b4...p1689-frontend)

A forward-port of this work onto `main` is in progress on
[`p1689-frontend-main-wip`](https://github.com/silvanshade/llvm-p1689-deps/tree/p1689-frontend-main-wip);
it currently carries only the library extraction and the driver options, not the
emitters. This branch is the complete implementation.

## Why

Tools that cache or schedule C++20 module builds need the module dependency graph.
Today they either run `clang-scan-deps` as an extra pass over every translation unit,
or they reconstruct dependencies by parsing the compiler command line. The second
approach is what most tools do, and it is fragile: the flags differ between compilers
and between versions of the same compiler, and GCC leaves some of the information
implicit or in non-standard depfile entries.

P1689 is the specified interchange format for exactly this problem. The obstacle is
that Clang and GCC leave most of its optional fields unpopulated, so consumers cannot
rely on it and fall back to command-line reconstruction. This branch fixes the Clang
half at the source: the compiler already has the information while it is compiling, so
it should be the thing that writes it out.

Upstream context: [llvm/llvm-project#76726](https://github.com/llvm/llvm-project/issues/76726)
tracks moving scanning into the frontend.

## What changed

- **New `clang/lib/DependencyAnalysis` library.** Dependency-related code is lifted out
  of `clang/lib/Frontend`, which had accumulated several unrelated collectors and
  generators. This is the bulk of the churn in the diff and is mechanical.
- **`P1689DataCollector`, `P1689DataFormatter`, `P1689FileGenerator`** — collect and
  emit P1689R5, populating the fields the specification defines, with room for vendor
  extensions.
- **`StructuredDependencyFile`** and `StructuredDependencyOutputOptions`, alongside the
  existing depfile options rather than replacing them.
- **Driver plumbing** in `Driver/ToolChains/Clang.cpp` for the `-fdeps*` options.

## Deliberately out of scope

Everything in `clang-scan-deps` that is *not* P1689: `compile_commands.json` discovery,
the scanning-service and VFS caching layers, module-build scheduling.
`clang-scan-deps` should continue to exist as a separate tool. The narrow claim here is
that the **P1689 subset does not require a second pass over the source**.

## Known gaps

The remaining difficulty is naming, and it is a Clang behaviour question rather than a
P1689 one:

- Clang does not output a `.pcm` by default when the preprocessor detects a module, and
  has no way to select a suitable default `.pcm` name. The existing naming algorithm
  does not account for partitions.
- Clang cannot infer a default `.pcm` name for *imports*, which is why `-fmodule-file=`
  must always be passed explicitly. GCC does not require this.
- Both of the above are what make P1689's `"compiled-module-path"` and `"source-path"`
  awkward to populate: that information is not available unless modules are loaded from
  AST files, and those do not exist unless we are actually compiling. Matching GCC's
  naming schema would let both compilers be driven identically.

Also outstanding: string parameter lifetimes want tightening, and the work needs tests
and documentation before it is proposable upstream.

## Orientation for reviewers

The driver-side entry points this work threads through, roughly in call order:

- `Driver::BuildCompilation` → `Driver::BuildInputs` → `Driver::handleArguments`
- `Driver::getFinalPhase`, `Driver::ConstructPhaseAction` (see `case phases::Preprocess`)
- `Clang::AddPreprocessingOptions` — the most useful place to start reading
- `parseFrontendArgs`, `Compilation::initCompilationForDiagnostics`
- `DependencyFileGenerator::outputDependencyFile`

Worth comparing while reading: `clang::ModuleDependencyCollector` (Frontend) against
`clang::tooling::dependencies::ModuleDepCollector` (DependencyScanning) — the overlap
between them is part of the motivation for the new library.

## Build and try

```sh
cmake -S llvm -B build -G Ninja \
  -DLLVM_ENABLE_PROJECTS=clang \
  -DCMAKE_BUILD_TYPE=Release
ninja -C build clang
./build/bin/clang++ -std=c++20 -fdeps-file=out.ddi -fdeps-format=p1689r5 -c mod.cppm
```

## License

Inherits the LLVM project license (Apache-2.0 with LLVM exceptions).
