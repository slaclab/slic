EXTERNALPROJECT_ADD(
    
    XercesC
    
    URL "http://apache.cs.utah.edu/xerces/c/3/sources/xerces-c-3.2.2.tar.gz"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/xerces"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/xerces/configure" --enable-shared --disable-static --prefix=${DEPENDENCY_INSTALL_DIR}/xerces
)

set( XercesC_DIR ${DEPENDENCY_INSTALL_DIR}/xerces CACHE PATH "XercesC install dir" FORCE )
SET( XercesC_INCLUDE_DIR ${DEPENDENCY_INSTALL_DIR}/xerces/include CACHE PATH "XercesC include dir" FORCE )
SET( XercesC_LIBRARY ${DEPENDENCY_INSTALL_DIR}/xerces/lib/libxerces-c.so CACHE PATH "XercesC library" FORCE )
