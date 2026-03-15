# SweepMiner

SweepMiner is a Minesweeper clone I'm working on to learn C++ and SDL3.

## Status
The game is fully functional and can be played all the way through. Some of the menu options are not implemented yet, and there may be some minor polishing left to do.

## Building

### Requirements
| Requirement  | Location                                                                                                   |
|--------------|------------------------------------------------------------------------------------------------------------|
| CMake        | [CMake](https://cmake.org/download/)                                                                       |
| C++ Compiler | [GCC](https://gcc.gnu.org/), [Clang](https://clang.llvm.org/), [MSVC](https://visualstudio.microsoft.com/) |

### Commands

```bash
# Windows (tested with MSVC, should work with MinGW too)
cmake --preset windows-release
cmake --build ./build/windows-release

# Linux (tested with GCC, should work with Clang too)
cmake --preset linux-release
cmake --build ./build/linux-release

# macOS (tested with Clang)
cmake --preset macos-release
cmake --build ./build/macos-release
```

The binary will be at `build/{preset}/Release/SweepMiner`.
