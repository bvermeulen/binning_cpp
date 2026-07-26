cmake_minimum_required(VERSION 3.12)

project(binning_cmake)
set(CMAKE_CXX_STANDARD 20)
enable_language(C CXX)

set(sources
    main.cpp
    config.cpp
    bins.cpp
    traces.cpp
    read_parse_sps.cpp
    data_handling.cpp
)

include_directories(includes
    ../include
    d:/cpp/boost_1_91_0
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)
add_executable(binning_cmake ${sources} ${includes})

find_library(SQLITE3_LIBRARY NAMES sqlite3)
target_link_libraries (binning_cmake sqlite3)

set_target_properties(binning_cmake PROPERTIES LDFLAGS "-Wall")
set_target_properties(binning_cmake PROPERTIES COMPILE_FLAGS "-Wall -g -fvisibility=hidden")
