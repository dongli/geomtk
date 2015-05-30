#
# FindUDUNITS
# -----------
#
# Find UDUNITS (version 2+), a library for manipulating and converting the
# units of physical quantities.
#
# Possible input environment variables:
#
#   UDUNITS_ROOT - Root directory of UDUNITS.
#
# Output CMake variables:
#
# 	UDUNITS_INCLUDE_DIRS
#	UDUNITS_LIBRARY_DIRS
#   UDUNITS_LIBRARIES
#
# Authors:
#
#	- Li Dong <dongli@lasg.iap.ac.cn>
#

if (DEFINED ENV{UDUNITS_ROOT})
	# Respect user specified location.
	set (UDUNITS_ROOT $ENV{UDUNITS_ROOT})
else ()
	# Find in system locations.

endif ()

set (UDUNITS_INCLUDE_DIRS "${UDUNITS_ROOT}/include")
set (UDUNITS_LIBRARY_DIRS "${UDUNITS_ROOT}/lib")
find_library (UDUNITS_LIBRARIES
    NAMES "udunits2"
    HINTS ${UDUNITS_LIBRARY_DIRS}
)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (UDUNITS
    FOUND_VAR UDUNITS_FOUND
    REQUIRED_VARS UDUNITS_INCLUDE_DIRS UDUNITS_LIBRARY_DIRS UDUNITS_LIBRARIES

)