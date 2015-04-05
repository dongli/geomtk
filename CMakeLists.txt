# CMake build script for GEOMTK.
#
# Copyright Li Dong 2013.
#
# Authors:
#   - Li Dong <dongli@lasg.iap.ac.cn>

cmake_minimum_required (VERSION 2.8)

# Check if this project is used as a submodule.
if (DEFINED PROJECT_NAME)
    message ("@@ GEOMTK is used as a submodule.")
    set (use_as_submodule TRUE)
endif ()

project (geomtk CXX)

option (OPENMP "Turn OpenMP compiler flag ON or OFF" OFF)
option (SHARED "Turn building shared libraries ON of OFF" OFF)

if (OPENMP)
    message ("@@ LASM uses OpenMP compiler flag.")
    find_package (OpenMP)
    if (OPENMP_FOUND)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    else ()
        message (WARNING "CXX compiler does not support OpenMP!")
    endif ()
else ()
    message ("@@ LASM does not use OpenMP compiler flag.")
endif ()
if (SHARED)
    set (shared_or_static SHARED)
else ()
    set (shared_or_static STATIC)
endif ()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Handle the external libraries.
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
# HDF5
set (HDF5_DIR $ENV{HDF5_ROOT})
find_package (HDF5)
include_directories (${HDF5_INCLUDE_DIRS})
# Boost
if (DEFINED ENV{BOOST_ROOT})
    message ("@@ Use user provided library Boost.")
    message ("@@ BOOST_ROOT = $ENV{BOOST_ROOT}")
    set (CMAKE_FIND_PREFIX_PATH $ENV{BOOST_ROOT})
endif ()
find_package (Boost 1.55.0 REQUIRED
    COMPONENTS regex filesystem system timer
)
if (NOT Boost_FOUND)
    message (FATAL_ERROR
        "CMake couldn't find library Boost! "
        "If it have been installed and you know where it is, "
        "set BOOST_ROOT (e.g. in .bashrc) to it."
    )
endif ()
include_directories (${Boost_INCLUDE_DIRS})
# NETCDF
list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
find_package (NETCDF REQUIRED COMPONENTS C)
include_directories (${NETCDF_INCLUDE_DIRS})
# MLPACK
if (DEFINED ENV{MLPACK_ROOT})
    message ("@@ Use user provided library MLPACK.")
    message ("@@ MLPACK_ROOT = $ENV{MLPACK_ROOT}")
    set (MLPACK_INCLUDE_DIRS "$ENV{MLPACK_ROOT}/include")
    set (MLPACK_LIBRARY_DIRS "$ENV{MLPACK_ROOT}/lib")
    find_package(LibXml2 2.6.0 REQUIRED)
    include_directories (${MLPACK_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIR})
    link_directories (${MLPACK_LIBRARY_DIRS})
else ()
    message (FATAL_ERROR
        "CMake couldn't find library MLPACK! "
        "If it have been installed and you know where it is, "
        "set MLPACK_ROOT (e.g. in .bashrc) to it."
    )
endif ()

# Set the source directory structure.
set (source_directories
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/src/geomtk"
    "${PROJECT_SOURCE_DIR}/src/Domain"
    "${PROJECT_SOURCE_DIR}/src/Mesh"
    "${PROJECT_SOURCE_DIR}/src/Field"
    "${PROJECT_SOURCE_DIR}/src/Regrid"
    "${PROJECT_SOURCE_DIR}/src/Filter"
    "${PROJECT_SOURCE_DIR}/src/Utils"
)

# Record the source directories and library include directories for upper project.
if (use_as_submodule)
    set (GEOMTK_INCLUDE_DIRS
        ${source_directories}
        ${ARMADILLO_INCLUDE_DIRS}
        ${HDF5_INCLUDE_DIRS}
        ${NETCDF_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${MLPACK_INCLUDE_DIRS}
        ${LIBXML2_INCLUDE_DIR}
        PARENT_SCOPE
    )
    set (GEOMTK_LIBRARY_DIRS
        ${MLPACK_LIBRARY_DIRS}
        PARENT_SCOPE
    )
endif ()

# Collect sources and headers.
foreach (dir ${source_directories})
    include_directories ("${dir}")
    # header files
    file (GLOB tmp1 "${dir}/*.h")
    list (APPEND headers ${tmp1})
    # source files
    aux_source_directory ("${dir}" tmp2)
    list (APPEND sources ${tmp2})
endforeach ()

# Define library targets.
add_library (geomtk ${shared_or_static} ${headers} ${sources})
target_link_libraries (geomtk
    ${ARMADILLO_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${NETCDF_LIBRARIES}
    ${Boost_LIBRARIES}
    mlpack
)

# Define install rules.
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

# Define uninstall rules.
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

# Define testing target
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
