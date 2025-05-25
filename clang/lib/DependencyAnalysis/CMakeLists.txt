set(LLVM_LINK_COMPONENTS
  Support
  )

add_clang_library(clangDependencyAnalysis
  DependencyCollector.cpp
  DepFileGenerator.cpp
  HeaderIncludeGenerator.cpp
  ModuleFileCollector.cpp
  P1689DataFormatter.cpp
  P1689FileGenerator.cpp
  StructuredDependencyFile.cpp

  LINK_LIBS
  clangBasic
  clangLex
  clangSerialization
  )
