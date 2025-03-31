
cmake_minimum_required(VERSION 3.5)

project(Arduino LANGUAGES CXX VERSION 0.0.1)
add_subdirectory(lib/Catch2)
# find_package(lib/Catch2 REQUIRED)

set( TESTS
     tests/demo.spec.cpp
     tests/utils.test.cpp
)

add_executable(unitTest tests/specs.cpp ${TESTS})

# add_library(volumegate volumegate/effect_compressor.cpp)
add_library(demo examples/demo.cpp)
add_library(volumegate volumegate/utils.cpp)
target_link_libraries(unitTest Catch2::Catch2WithMain volumegate)
target_link_libraries(unitTest Catch2::Catch2WithMain demo)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
# catch_discover_tests(demo)
# catch_discover_tests(volumegate)
