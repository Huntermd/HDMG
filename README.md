# HDMG
A gb emulator written in C++ with SDL and CLI support.

## Features
- Supports DMG games
- Has basic no MBC / MBC1/ MBC3/ MBC5 support(A good link for more information) https://gbhwdb.gekkio.fi/cartridges/gb.html
- CLI support

## Build Instructions

### Prequisties
- CMake
- C++17 or later compiler
- SDL2
### Steps
```bash
    git clone https://github.com/Huntermd/HDMG.git
    cd HDMG
    cmake -B build
    cmake --build build
```
## How to use
```bash
    /HDMG path/to/game.gb
```
## Controls
- Arrow Keys = D-pad
- A
- B
- S = Start
- D = Select
## Future Features
- GBC support
- APU support