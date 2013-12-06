cmake_minimum_required (VERSION 2.6)

project (geomtk CXX)

include (CTest)

add_subdirectory ("${PROJECT_SOURCE_DIR}/external/gtest-1.7.0")
add_subdirectory ("${PROJECT_SOURCE_DIR}/src")
