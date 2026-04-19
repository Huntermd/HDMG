# HDMG
A Game Boy / Game Boy Color emulator written in C++ with SDL2.

## Prerequisites
- CMake 3.16+
- A C++20 compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- SDL2

## Build

### Linux
```bash
sudo apt install cmake ninja-build libsdl2-dev   # or your distro's equivalent
git clone https://github.com/Huntermd/HDMG.git
cd HDMG
cmake --preset linux-release
cmake --build build/linux-release
```

### Windows (vcpkg)
```bat
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
C:\vcpkg\vcpkg install sdl2
setx VCPKG_ROOT C:\vcpkg

git clone https://github.com/Huntermd/HDMG.git
cd HDMG
cmake --preset windows-release
cmake --build build\windows-release
```

### macOS
```bash
brew install cmake ninja sdl2
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Usage
```bash
./HDMG path/to/rom.gb     # or .gbc
```

## Controls
- Arrow keys — D-pad
- A — A
- B — B
- S — Start
- D — Select
