EXTERNALPROJECT_ADD(
    
    XercesC
    
    URL "https://downloads.apache.org/xerces/c/3/sources/xerces-c-3.2.3.tar.gz"

    SOURCE_DIR "${CMAKE_BINARY_DIR}/xerces"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/xerces/configure" --enable-shared --disable-static --prefix=${DEPENDENCY_INSTALL_DIR}/xerces
)

set( XercesC_DIR ${DEPENDENCY_INSTALL_DIR}/xerces CACHE PATH "XercesC install dir" FORCE )
SET( XercesC_INCLUDE_DIR ${DEPENDENCY_INSTALL_DIR}/xerces/include CACHE PATH "XercesC include dir" FORCE )
SET( XercesC_LIBRARY ${DEPENDENCY_INSTALL_DIR}/xerces/lib/libxerces-c.so CACHE PATH "XercesC library" FORCE )
SET( XercesC_VERSION "3.2.2" CACHE STRING "XercesC version" FORCE)
