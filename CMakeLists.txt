cmake_minimum_required(VERSION 3.10)
project(aiko)

set(CMAKE_CXX_STANDARD 17)

# Output binary ke folder bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Folder include untuk header
include_directories(include)

# Ambil semua cpp di src/
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Tambahkan main.cpp yang ada di root + semua sources
add_executable(aiko main.cpp ${SOURCES})
