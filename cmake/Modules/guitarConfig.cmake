INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_GUITAR guitar)

FIND_PATH(
    GUITAR_INCLUDE_DIRS
    NAMES guitar/api.h
    HINTS $ENV{GUITAR_DIR}/include
        ${PC_GUITAR_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GUITAR_LIBRARIES
    NAMES gnuradio-guitar
    HINTS $ENV{GUITAR_DIR}/lib
        ${PC_GUITAR_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GUITAR DEFAULT_MSG GUITAR_LIBRARIES GUITAR_INCLUDE_DIRS)
MARK_AS_ADVANCED(GUITAR_LIBRARIES GUITAR_INCLUDE_DIRS)

