find_path(LCIO_INCLUDE_DIRS lcio.h ${LCIO_DIR}/include)

find_library(LCIO_LIBRARY lcio ${LCIO_DIR}/lib)
find_library(SIO_LIBRARY sio ${LCIO_DIR}/lib)
set(LCIO_LIBRARIES ${LCIO_LIBRARY} ${SIO_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args( LCIO DEFAULT_MSG LCIO_LIBRARIES LCIO_INCLUDE_DIRS )
