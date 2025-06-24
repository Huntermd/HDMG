cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
project("HDMG")
find_package(SDL2)

add_executable (HDMG "Main.cpp" "GameBoy.cpp" "GameBoy.h" "Cartridge.cpp" "Cartridge.h" "PPU.cpp" "PPU.h" "Mapper.cpp" "Mapper.h")


target_link_libraries(CmakeTest PRIVATE SDL2::SDL2 SDL2::SDL2main)