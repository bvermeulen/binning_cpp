cmake_minimum_required(VERSION 3.12)

project(binning_cmake)
set(CMAKE_CXX_STANDARD 20)
enable_language(C CXX)

set(sources
    binning.cpp
    bins.cpp
    read_parse_sps.cpp
    save_data.cpp
)

include_directories(includes
    ../include
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/debug)
add_executable(binning_cmake ${sources} ${includes})

find_library(SQLITE3_LIBRARY NAMES sqlite3)
target_link_libraries (binning_cmake sqlite3)

set_target_properties(binning_cmake PROPERTIES LDFLAGS "-Wall")
set_target_properties(binning_cmake PROPERTIES COMPILE_FLAGS "-Wall -g -fvisibility=hidden")
