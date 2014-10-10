#
# FindNETCDF
# ----------
#
# Find NETCDF, a set of software libraries and self-describing, machine-independent data formats.
#
# Possible input environment variables:
#
#   NETCDF_ROOT - Root directory of NETCDF (all language interfaces reside there).
#   NETCDF_C_ROOT - Root directory of NETCDF C interface.
#   NETCDF_CXX_ROOT - Root directory of NETCDF CXX interface.
#   NETCDF_FORTRAN_ROOT - Root directory of NETCDF Fortran interface.
#
# Output CMake variables:
#
#   NETCDF_INCLUDE_DIRS - List of all the NETCDF includes (with all interfaces) and the dependency includes.
#   NETCDF_LIBRARY_DIRS - List of all the NETCDF libs (with all interfaces) and the dependency libs.
#   NETCDF_LIBRARIES - List of all the NETCDF libraries (shared or static) and the dependency libraries.
#
# Authors:
#
#   - Li Dong <dongli@lasg.iap.ac.cn>
#

if (${NETCDF_FIND_REQUIRED})
    set (required_or_not REQUIRED)
endif ()
if (${NETCDF_FIND_QUIETLY})
    set (quiet_or_not QUIET)
endif ()

# NETCDF C interface must be found.
list (FIND NETCDF_FIND_COMPONENTS "C" res)
if (res EQUAL -1)
    list (APPEND NETCDF_FIND_COMPONENTS "C")
endif ()

set (netcdf_interfaces "C" "CXX" "Fortran")
set (netcdf_config_command "nc-config" "ncxx4-config" "nf-config")
set (netcdf_libraries "netcdf" "netcdf_c++4" "netcdff")
foreach (i RANGE 0 2)
    list (GET netcdf_interfaces ${i} lang)
    list (GET netcdf_config_command ${i} config_command)
    list (GET netcdf_libraries ${i} lib)
    list (FIND NETCDF_FIND_COMPONENTS ${lang} res)
    if (NOT res EQUAL -1)
        find_program (${lang}_config NAMES ${config_command})
        if (${lang}_config MATCHES "NOTFOUND")
            foreach (var IN ITEMS "NETCDF_ROOT" "NETCDF_${lang}_ROOT")
                if (DEFINED ENV{${var}})
                    set (NETCDF_${lang}_ROOT $ENV{${var}})
                    break ()
                endif ()
            endforeach ()
            if (DEFINED NETCDF_${lang}_ROOT)
                set (${lang}_config "${NETCDF_${lang}_ROOT}/bin/${config_command}")
            endif ()
        else ()
            get_filename_component (bin ${${lang}_config} PATH)
            string (REGEX REPLACE "/bin$" "" NETCDF_${lang}_ROOT ${bin})
        endif ()
        if (DEFINED NETCDF_${lang}_ROOT)
            list (APPEND NETCDF_${lang}_INCLUDE_DIRS "${NETCDF_${lang}_ROOT}/include")
            list (APPEND NETCDF_${lang}_LIBRARY_DIRS "${NETCDF_${lang}_ROOT}/lib")
            if (NETCDF_USE_STATIC)
                find_library (NETCDF_${lang}_LIBRARIES
                    NAMES lib${lib}.a
                    HINTS ${NETCDF_${lang}_LIBRARY_DIRS}
                )
            else ()
                find_library (NETCDF_${lang}_LIBRARIES
                    NAMES ${lib}
                    HINTS ${NETCDF_${lang}_LIBRARY_DIRS}
                )
            endif ()
            # Get version string.
            execute_process (COMMAND ${${lang}_config} --version OUTPUT_VARIABLE output)
            string (REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" NETCDF_${lang}_VERSION_STRING ${output})
        else ()
            set (NETCDF_LACK_INTERFACE TRUE)
        endif ()
    endif ()
endforeach ()

if (NOT NETCDF_LACK_INTERFACE)
    set (NETCDF_ALL_FOUND TRUE)
endif ()

execute_process (COMMAND nc-config --has-pnetcdf OUTPUT_VARIABLE output)
if (output MATCHES "yes")
    find_package (PNETCDF ${quiet_or_not} ${required_or_not})
    if (PNETCDF_FOUND)
        list (APPEND NETCDF_INCLUDE_DIRS ${PNETCDF_INCLUDE_DIRS})
        list (APPEND NETCDF_LIBRARY_DIRS ${PNETCDF_LIBRARY_DIRS})
        list (APPEND NETCDF_LIBRARIES ${PNETCDF_LIBRARIES})
    endif ()
endif ()

foreach (lang IN LISTS netcdf_interfaces)
    if (DEFINED NETCDF_${lang}_INCLUDE_DIRS)
        list (APPEND NETCDF_INCLUDE_DIRS ${NETCDF_${lang}_INCLUDE_DIRS})
        list (APPEND NETCDF_LIBRARY_DIRS ${NETCDF_${lang}_LIBRARY_DIRS})
        list (APPEND NETCDF_LIBRARIES ${NETCDF_${lang}_LIBRARIES})
    endif ()
endforeach ()
list (REMOVE_DUPLICATES NETCDF_INCLUDE_DIRS)
list (REMOVE_DUPLICATES NETCDF_LIBRARY_DIRS)
list (REMOVE_DUPLICATES NETCDF_LIBRARIES)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (NETCDF FOUND_VAR NETCDF_FOUND
    REQUIRED_VARS NETCDF_ALL_FOUND NETCDF_INCLUDE_DIRS NETCDF_LIBRARY_DIRS NETCDF_LIBRARIES
)
