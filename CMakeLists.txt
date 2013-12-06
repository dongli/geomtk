cmake_minimum_required (VERSION 2.6)

project (geomtk CXX)

include (CTest)

# ------------------------------------------------------------------------------
# external dependencies
if (CMAKE_BUILD_TYPE MATCHES DEBUG)
	add_subdirectory ("${PROJECT_SOURCE_DIR}/external/gtest-1.7.0")
endif ()

find_package (Armadillo)
if (NOT ARMADILLO_FOUND)
    if (DEFINED ENV{ARMADILLO_ROOT})
        message ("@@ Use user provided library Armadillo!")
        message ("@@ ARMADILLO_ROOT = $ENV{ARMADILLO_ROOT}")
        find_package (Armadillo HINTS $ENV{ARMADILLO_ROOT})
    else ()
        message (FATAL_ERROR "CMake couldn't find library Armadillo! "
                             "If it have been installed and you know where it is, "
                             "set ARMADILLO_ROOT (e.g. in .bashrc) to it.")
    endif ()
endif ()

# ------------------------------------------------------------------------------
# internal sources
set (source_dirs
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/src/Domain"
    "${PROJECT_SOURCE_DIR}/src/Mesh"
    "${PROJECT_SOURCE_DIR}/src/Field"
    "${PROJECT_SOURCE_DIR}/src/Regrid"
)

foreach (dir ${source_dirs})
    # source files
    aux_source_directory ("${dir}" tmp1)
    list (APPEND sources ${tmp1})
    include_directories ("${dir}")
    # header files
    execute_process (COMMAND
        find ${dir} -maxdepth 1 -name "*.h"
        OUTPUT_VARIABLE tmp2
    )
    list (APPEND headers ${tmp2})
endforeach ()
string (REGEX REPLACE "\n" ";" headers ${headers})

include_directories (${ARMADILLO_INCLUDE_DIRS})
add_library (geomtk STATIC ${sources})
target_link_libraries (geomtk armadillo)

install (TARGETS geomtk
    ARCHIVE DESTINATION lib
)
foreach (header IN ITEMS ${headers})
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
# testing
if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    enable_testing ()
    
    foreach (dir ${SRC_DIRS})
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

