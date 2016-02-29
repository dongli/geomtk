#
# FindMLPACK
# ----------
#
# Find MLPACK, a scalable C++ machine learning library.
#
# Possible input environment variables:
#
#   MLPACK_ROOT - Root directory of MLPACK.
#
# Output CMake variables:
#
#   MLPACK_INCLUDE_DIRS
#   MLPACK_LIBRARY_DIRS
#   MLPACK_LIBRARIES
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

# Find libmlpack.* 
if (APPLE)
    string (REPLACE ":" ";" search_paths $ENV{DYLD_LIBRARY_PATH})
elseif (UNIX)
    string (REPLACE ":" ";" search_paths $ENV{LD_LIBRARY_PATH})
endif ()
find_library (MLPACK_LIBRARIES mlpack PATHS ${search_paths})
string (REGEX REPLACE "/lib/libmlpack.*" "" MLPACK_ROOT ${MLPACK_LIBRARIES})
set (MLPACK_INCLUDE_DIRS "${MLPACK_ROOT}/include")
set (MLPACK_LIBRARY_DIRS "${MLPACK_ROOT}/lib")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (MLPACK FOUND_VAR MLPACK_FOUND
    REQUIRED_VARS MLPACK_INCLUDE_DIRS MLPACK_LIBRARY_DIRS MLPACK_LIBRARIES
)
