set(LLVM_LINK_COMPONENTS
  Support
  )

add_clang_library(clangDependencyAnalysis
  DependencyFile.cpp
  HeaderIncludeGen.cpp
  ModuleDependencyCollector.cpp

  LINK_LIBS
  clangBasic
  clangLex
  clangSerialization
  )
