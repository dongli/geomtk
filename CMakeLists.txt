# ------------------------------------------------------------------------------
# CMake build script for GeoMTK.
#
# Copyright 2013.
#
# Authors:
#   - Li Dong <dongli@lasg.iap.ac.cn>
# ------------------------------------------------------------------------------

cmake_minimum_required (VERSION 2.8)

project (geomtk CXX)

# ------------------------------------------------------------------------------
# source directory structure
set (source_directories
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/src/Domain"
    "${PROJECT_SOURCE_DIR}/src/Mesh"
    "${PROJECT_SOURCE_DIR}/src/Field"
    "${PROJECT_SOURCE_DIR}/src/Regrid"
)

# ------------------------------------------------------------------------------
# collect sources and headers
foreach (dir ${source_directories})
    include_directories ("${dir}")
    # source files
    aux_source_directory ("${dir}" tmp1)
    list (APPEND sources ${tmp1})
    # header files
    file (GLOB tmp2 "${dir}/*.h")
    list (APPEND headers ${tmp2})
endforeach ()

# ------------------------------------------------------------------------------
# external libraries
find_package (Armadillo)
if (NOT ARMADILLO_FOUND)
    if (DEFINED ENV{ARMADILLO_ROOT})
        message ("@@ Use user provided library Armadillo!")
        message ("@@ ARMADILLO_ROOT = $ENV{ARMADILLO_ROOT}")
        find_package (Armadillo HINTS $ENV{ARMADILLO_ROOT})
    else ()
        message (FATAL_ERROR
            "CMake couldn't find library Armadillo! "
            "If it have been installed and you know where it is, "
            "set ARMADILLO_ROOT (e.g. in .bashrc) to it."
        )
    endif ()
endif ()

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package (NETCDF)
if (NOT NETCDF_FOUND)
    if (DEFINED ENV{NETCDF_ROOT})
        message ("@@ Use user provided library NetCDF!")
        message ("@@ NETCDF_ROOT = $ENV{NETCDF_ROOT}")
        find_package (NETCDF HINTS $ENV{NETCDF_ROOT})
    else ()
        message (FATAL_ERROR
            "CMake couldn't find library NetCDF! "
            "If it have been installed and you know where it is, "
            "set NETCDF_ROOT (e.g. in .bashrc) to it."
        )
    endif ()
endif ()

# ------------------------------------------------------------------------------
# library targets
add_library (geomtk STATIC ${sources})
include_directories (
    ${ARMADILLO_INCLUDE_DIRS}
    ${NETCDF_INCLUDE_DIRS}
)
target_link_libraries (geomtk
    ${ARMADILLO_LIBRARIES}
    ${NETCDF_LIBRARIES}
)

# ------------------------------------------------------------------------------
# install rules
install (TARGETS geomtk
    EXPORT geomtk-targets
    ARCHIVE DESTINATION lib
)
install (EXPORT geomtk-targets
    DESTINATION lib/geomtk
)
foreach (header ${headers})
    if (${header} MATCHES "geomtk.h")
        install (FILES ${header}
            DESTINATION "include"
        )
    else ()
        install (FILES ${header}
            DESTINATION "include/geomtk"
        )
    endif ()
endforeach ()

# ------------------------------------------------------------------------------
# uninstall rules
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
)
add_custom_target (uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

# ------------------------------------------------------------------------------
# testing target
if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    include (CTest)
    enable_testing ()
    add_subdirectory ("${PROJECT_SOURCE_DIR}/external/gtest-1.7.0")

    foreach (dir ${source_directories})
        if (EXISTS "${dir}/test")
            include_directories ("${dir}/test")
        endif (EXISTS "${dir}/test")
    endforeach ()
    include_directories (${gtest_SOURCE_DIR} ${gtest_SOURCE_DIR}/include)

    add_executable (geomtk_test "${PROJECT_SOURCE_DIR}/src/test/test.cpp")
    target_link_libraries (geomtk_test gtest gtest_main geomtk)

    set_target_properties (geomtk geomtk_test
        PROPERTIES COMPILE_FLAGS "-DUNIT_TEST"
    )
    add_test (
        NAME geomtk_test
        COMMAND geomtk_test
    )
endif ()
