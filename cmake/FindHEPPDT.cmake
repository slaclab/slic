FIND_PATH( HEPPDT_INCLUDE_DIR HepPDT/Version.hh ${HEPPDT_DIR}/include )

FIND_LIBRARY( HEPPDT_LIBRARY HepPDT ${HEPPDT_DIR}/src/HepPDT/.libs/  )
FIND_LIBRARY( HEPPID_LIBRARY HepPID ${HEPPDT_DIR}/src/HepPID/.libs/ )

SET( HEPPDT_LIBRARIES ${HEPPDT_LIBRARY} ${HEPPID_LIBRARY} )

# Report the found libraries, quit with fatal error if any required library has not been found.
INCLUDE( FindPackageHandleStandardArgs )

FIND_PACKAGE_HANDLE_STANDARD_ARGS( HEPPDT DEFAULT_MSG HEPPDT_LIBRARIES HEPPDT_INCLUDE_DIR )
