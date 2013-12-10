# - Find NETCDF
# Find the NETCDF library
#
# Using NETCDF:
#  find_package (NETCDF REQUIRED)
#  include_directories (${NETCDF_INCLUDE_DIRS})
#  add_executable (foo foo.cpp)
#  target_link_libraries (foo ${NETCDF_LIBRARIES})
# This module sets the following variables:
#  NETCDF_FOUND - set to true if the library is found
#  NETCDF_INCLUDE_DIRS - list of required include directories
#  NETCDF_C_LIBRARIES - list of C libraries to be linked
#  NETCDF_Fortran_LIBRARIES - list of Fortran libraries to be linked
#  NETCDF_VERSION_MAJOR - major version number
#  NETCDF_VERSION_MINOR - minor version number
#  NETCDF_VERSION_PATCH - patch version number
#
# Authors
#  - Li Dong <dongli@lasg.iap.ac.cn>
# ------------------------------------------------------------------------------
# try to find the nc-config command
find_program (NETCDF_NCCONFIG_COMMAND
    NAME nc-config
    HINTS ENV NETCDF_ROOT
    PATH_SUFFIXES bin
    DOC "NETCDF nc-config command. Used only to detect NETCDF root."
)
mark_as_advanced (NETCDF_NCCONFIG_COMMAND)
if (NOT NETCDF_NCCONFIG_COMMAND)
    if (DEFINED ENV{NETCDF_ROOT})
        set (NETCDF_NCCONFIG_COMMAND $ENV{NETCDF_ROOT}/bin/nc-config)
    else ()
        message (FATAL_ERROR "Could not find NETCDF!")
    endif ()
endif ()
set (NETCDF_FOUND TRUE)
# ------------------------------------------------------------------------------
# set NETCDF root
get_filename_component (
    NETCDF_ROOT
    ${NETCDF_NCCONFIG_COMMAND}
    DIRECTORY
)
get_filename_component (
    NETCDF_ROOT
    ${NETCDF_ROOT}
    DIRECTORY
)
# ------------------------------------------------------------------------------
# get version
execute_process (
    COMMAND ${NETCDF_NCCONFIG_COMMAND} --version
    OUTPUT_VARIABLE output
    RESULT_VARIABLE result
    ERROR_VARIABLE error
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string (REGEX REPLACE "netCDF *" "" tmp ${output})
string (REPLACE "." ";" tmp ${tmp})
list (GET tmp 0 NETCDF_VERSION_MAJOR)
list (GET tmp 1 NETCDF_VERSION_MINOR)
list (GET tmp 2 NETCDF_VERSION_PATCH)
# ------------------------------------------------------------------------------
# check which components does NETCDF have
set (NETCDF_COMPONENTS "C")
execute_process (
    COMMAND ${NETCDF_NCCONFIG_COMMAND} --has-c++4
    OUTPUT_VARIABLE output
    RESULT_VARIABLE result
    ERROR_VARIABLE error
)
if (${output} MATCHES "yes")
    list (APPEND NETCDF_COMPONENTS "CXX")
endif ()
execute_process (
    COMMAND ${NETCDF_NCCONFIG_COMMAND} --has-f90
    OUTPUT_VARIABLE output
    RESULT_VARIABLE result
    ERROR_VARIABLE error
)
if (${output} MATCHES "yes")
    list (APPEND NETCDF_COMPONENTS "Fortran")
endif ()
# ------------------------------------------------------------------------------
# set NETCDF include and libraries
set (NETCDF_INCLUDE_DIRS ${NETCDF_ROOT}/include)
list (FIND NETCDF_FIND_COMPONENTS "C" find_C)
if (NOT ${find_C} EQUAL -1)
    set (NETCDF_C_LIBRARIES "-L${NETCDF_ROOT}/lib -lnetcdf")
endif ()
list (FIND NETCDF_FIND_COMPONENTS "Fortran" find_Fortran)
list (FIND NETCDF_COMPONENTS "Fortran" has_Fortran)
if (NOT ${find_Fortran} EQUAL -1)
    if (${has_Fortran} EQUAL -1)
        message (FATAL_ERROR "NETCDF does not have Fortran API installed!")
    endif ()
    set (NETCDF_Fortran_LIBRARIES "${NETCDF_LIBRARIES} -lnetcdf -lnetcdff")
endif ()
# ------------------------------------------------------------------------------
# report 
message (STATUS
    "Found NETCDF ${NETCDF_VERSION_MAJOR}."
                 "${NETCDF_VERSION_MINOR}."
                 "${NETCDF_VERSION_PATCH} "
    "in ${NETCDF_ROOT}")
