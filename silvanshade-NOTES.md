# NOTES

- `phases::Preprocess`
- `ActionClass::PreprocessJobClass`

- `Clang::AddPreprocessingOptions`
  - probably start here

- `getCompilationPhases`
- `Compilation::initCompilationForDiagnostics`
- `phases::ID Driver::getFinalPhase`
- `Driver::BuildCompilation`
- `Driver::BuildInputs`
- `Driver::handleArguments`
- `parseFrontendArgs`
- `Driver::ConstructPhaseAction`
  - see `case phases::Preprocess`

- compare <Frontend> `clang::ModuleDependencyCollector` with <DependencyScanning> `clang::tooling::dependencies::ModuleDepCollector`

- `DependencyFileGenerator::outputDependencyFile`
