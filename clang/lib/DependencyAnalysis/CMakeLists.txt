set(LLVM_LINK_COMPONENTS
  Support
  )

add_clang_library(clangDependencyAnalysis
  P1689/P1689DataCollector.cpp
  P1689/P1689DataFormatter.cpp
  P1689/P1689FileGenerator.cpp
  DependencyCollector.cpp
  DepFileGenerator.cpp
  HeaderIncludeGenerator.cpp
  ModuleFileCollector.cpp
  StructuredDependencyFile.cpp

  LINK_LIBS
  clangBasic
  clangLex
  clangSerialization
  )
