find_path(LCDD_INCLUDE_DIR lcdd/core/LCDDProcessor.hh ${LCDD_DIR}/include)
find_library(LCDD_LIBRARY lcdd ${LCDD_DIR}/lib/)
find_package_handle_standard_args( LCDD DEFAULT_MSG LCDD_INCLUDE_DIR LCDD_LIBRARY)
