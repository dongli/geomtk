find_package (Armadillo)
if (Armadillo_FOUND)
    message (STATUS "DDD")
endif (Armadillo_FOUND)

set (SRC_DIRS
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/src/Domain"
    "${PROJECT_SOURCE_DIR}/src/Mesh"
    "${PROJECT_SOURCE_DIR}/src/Field"
    "${PROJECT_SOURCE_DIR}/src/Regrid"
)

set (SRCS "")
foreach (dir ${SRC_DIRS})
    aux_source_directory ("${dir}" tmp)
    set (SRCS ${SRCS} ${tmp})
    include_directories ("${dir}")
endforeach (dir)

add_library (geomtk STATIC ${SRCS})
target_link_libraries (geomtk armadillo)

install (TARGETS geomtk
    ARCHIVE DESTINATION lib
)
install (DIRECTORY "${PROJECT_SOURCE_DIR}/src/."
    DESTINATION "include/geomtk"
    FILES_MATCHING PATTERN "*.h"
                   PATTERN "geomtk.h" EXCLUDE
)
install (FILES "${PROJECT_SOURCE_DIR}/src/geomtk.h"
    DESTINATION "include"
)

# ------------------------------------------------------------------------------
# testing
if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    message ("In debug mode")

    enable_testing ()
    
    foreach (dir ${SRC_DIRS})
        if (EXISTS "${dir}/test")
            include_directories ("${dir}/test")
        endif (EXISTS "${dir}/test")
    endforeach (dir)
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
endif (CMAKE_BUILD_TYPE MATCHES DEBUG)
