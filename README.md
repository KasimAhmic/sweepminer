# SweepMiner

SweepMiner is a Minesweeper clone I'm working on to learn C++ and SDL3.

## Building

### Requirements
| Requirement  | Location                                                                                                   |
|--------------|------------------------------------------------------------------------------------------------------------|
| CMake        | [CMake](https://cmake.org/download/)                                                                       |
| C++ Compiler | [GCC](https://gcc.gnu.org/), [Clang](https://clang.llvm.org/), [MSVC](https://visualstudio.microsoft.com/) |
| vcpkg        | [vcpkg](https://github.com/microsoft/vcpkg)                                                                |

### Commands

Still new to this whole C++ thing, so I'm not sure if this is all you'll need, but these commands should work.

```bash
# Windows (mingw)
cmake --preset windows-release
cmake --build --preset build-windows-release

# Linux (tested with GCC, should work with Clang too)
cmake --preset linux-release
cmake --build --preset build-linux-release

# macOS (tested with Clang)
cmake --preset macos-release
cmake --build --preset build-macos-release
```

The binary will be at `build/{preset}/bin/SweepMiner`.
