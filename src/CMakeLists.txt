
cmake_minimum_required(VERSION 3.5)

project(Arduino LANGUAGES CXX VERSION 0.0.1)
add_subdirectory(lib/Catch2)

set( TESTS
     spec/effect_compressor.spec.cpp
)

add_executable(unitTest spec/specs.cpp ${TESTS})

add_library(volumegate volumegate/effect_compressor.cpp)
target_link_libraries(unitTest Catch2::Catch2 demo-library)
