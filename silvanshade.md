# NOTES

## Features

- Output .pcm file by default if preprocessor detects a module
- Select a suitable default .pcm name
  - The current algorithm does not account for partitions
  - Try to use the same schema as GCC
- Allow compiler to infer default .pcm name for imports
  - This would allow to avoid always specifying -fmodule-file=
  - This would bring Clang usage closer to GCC
- These changes will help compute names for the p1689 fields:
  - "compiled-module-path"
  - "source-path"
- This addresses a problem we would face with p1689 generation when attempting to determine these flags with the current compiler behavior. We cannot get this information unless the modules are loaded from the AST files which will not exist unless we are actually compiling.
