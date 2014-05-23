# ------------------------------------------------------------------------------
# CMake build script for GEOMTK.
#
# Copyright Li Dong 2013.
#
# Authors:
#   - Li Dong <dongli@lasg.iap.ac.cn>
# ------------------------------------------------------------------------------
cmake_minimum_required (VERSION 2.8)
# ------------------------------------------------------------------------------
# check if there is upper project
if (DEFINED PROJECT_NAME)
    message ("@@ GEOMTK is used as a submodule.")
    set (use_as_submodule TRUE)
endif ()
# ------------------------------------------------------------------------------
project (geomtk CXX)
# ------------------------------------------------------------------------------
# external libraries
# Armadillo
if (DEFINED ENV{ARMADILLO_ROOT})
    message ("@@ Use user provided library Armadillo.")
    message ("@@ ARMADILLO_ROOT = $ENV{ARMADILLO_ROOT}")
    find_package (Armadillo HINTS $ENV{ARMADILLO_ROOT})
else ()
    find_package (Armadillo)
    if (NOT ARMADILLO_FOUND)
        message (FATAL_ERROR
            "CMake couldn't find library Armadillo! "
            "If it have been installed and you know where it is, "
            "set ARMADILLO_ROOT (e.g. in .bashrc) to it."
        )
    endif ()
endif ()
include_directories (${ARMADILLO_INCLUDE_DIRS})
# Boost
find_package (Boost 1.55.0 REQUIRED COMPONENTS regex)
if (NOT Boost_FOUND)
    message (FATAL_ERROR
        "CMake couldn't find library Boost! "
    )
endif ()
include_directories (${Boost_INCLUDE_DIRS})
# NetCDF
list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
find_package (NETCDF COMPONENTS C)
include_directories (${NETCDF_INCLUDE_DIRS})
# ------------------------------------------------------------------------------
# source directory structure
set (source_directories
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/src/Domain"
    "${PROJECT_SOURCE_DIR}/src/Mesh"
    "${PROJECT_SOURCE_DIR}/src/Field"
    "${PROJECT_SOURCE_DIR}/src/Regrid"
    "${PROJECT_SOURCE_DIR}/src/Utils"
)
# record the source directories for upper project
if (use_as_submodule)
    set (GEOMTK_INCLUDE_DIRS
        ${source_directories}
        ${ARMADILLO_INCLUDE_DIRS}
        ${NETCDF_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        PARENT_SCOPE
    )
endif ()
# ------------------------------------------------------------------------------
# collect sources and headers
foreach (dir ${source_directories})
    include_directories ("${dir}")
    # header files
    file (GLOB tmp1 "${dir}/*.h")
    list (APPEND headers ${tmp1})
    # source files
    aux_source_directory ("${dir}" tmp2)
    list (APPEND sources ${tmp2})
endforeach ()
# ------------------------------------------------------------------------------
# library targets
add_library (geomtk STATIC ${headers} ${sources})
target_link_libraries (geomtk
    ${ARMADILLO_LIBRARIES}
    ${NETCDF_C_LIBRARIES}
    ${Boost_LIBRARIES}
)

# ------------------------------------------------------------------------------
# install rules
if (${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
    install (TARGETS geomtk
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        COMPONENT library
    )
    foreach (header ${headers})
        install (FILES ${header}
            DESTINATION "include/geomtk"
        )
    endforeach ()
endif ()
# ------------------------------------------------------------------------------
# uninstall rules
if (${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
    configure_file (
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )
    add_custom_target (uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif ()
# ------------------------------------------------------------------------------
# testing target
add_subdirectory ("${PROJECT_SOURCE_DIR}/external/gtest-1.7.0")

foreach (dir ${source_directories})
    if (EXISTS "${dir}/test")
        include_directories ("${dir}/test")
        file (GLOB tmp1 "${dir}/test/*.h")
        list (APPEND test_headers ${tmp1})
    endif (EXISTS "${dir}/test")
endforeach ()
include_directories (${gtest_SOURCE_DIR} ${gtest_SOURCE_DIR}/include)

add_executable (test_geomtk
    ${test_headers}
    "${PROJECT_SOURCE_DIR}/src/test/test.cpp"
)
set_target_properties (test_geomtk
    PROPERTIES COMPILE_FLAGS "-DUNIT_TEST"
)
target_link_libraries (test_geomtk
    ${NETCDF_LIBRARIES}
    gtest gtest_main
    geomtk
)
