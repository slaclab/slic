FIND_PATH( LCDD_INCLUDE_DIR lcdd/core/LCDDProcessor.hh ${LCDD_DIR}/include )

FIND_LIBRARY( LCDD_LIBRARY lcdd ${LCDD_DIR}/lib/ )

INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( LCDD DEFAULT_MSG LCDD_INCLUDE_DIR LCDD_LIBRARY )
