cmake_minimum_required(VERSION 3.16)
project(HDMG LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Release if the user didn't pick one
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

find_package(SDL2 REQUIRED)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/GameBoy)

add_executable(HDMG
    ${SRC_DIR}/Main.cpp
    ${SRC_DIR}/GameBoy.cpp
    ${SRC_DIR}/Cartridge.cpp
    ${SRC_DIR}/PPU.cpp
    ${SRC_DIR}/Mapper.cpp
    ${SRC_DIR}/MBC1.cpp
    ${SRC_DIR}/MBC2.cpp
    ${SRC_DIR}/MBC3.cpp
    ${SRC_DIR}/NMBC.cpp
)

target_include_directories(HDMG PRIVATE ${SRC_DIR} ${SDL2_INCLUDE_DIRS})

# SDL2main only exists on Windows. Guard it.
if(TARGET SDL2::SDL2main)
    target_link_libraries(HDMG PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(HDMG PRIVATE SDL2::SDL2)

# Warnings — would've caught the SBVK bug
if(MSVC)
    target_compile_options(HDMG PRIVATE /W4)
else()
    target_compile_options(HDMG PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()