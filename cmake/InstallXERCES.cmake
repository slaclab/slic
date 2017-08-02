EXTERNALPROJECT_ADD(
    
    XERCES
    
    URL "http://apache.cs.utah.edu/xerces/c/3/sources/xerces-c-3.1.4.tar.gz"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/xerces"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/xerces/configure" --enable-shared --disable-static --prefix=${DEPENDENCY_INSTALL_DIR}/xerces
)

SET( XERCES_DIR ${DEPENDENCY_INSTALL_DIR}/xerces CACHE PATH "XERCES install dir" FORCE )
