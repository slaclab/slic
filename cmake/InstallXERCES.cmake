EXTERNALPROJECT_ADD(
    
    XERCES
    
    URL "http://apache.cs.utah.edu//xerces/c/3/sources/xerces-c-3.1.4.tar.gz"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/extdep/xerces"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/extdep/xerces/configure" --enable-shared --disable-static --prefix=${CMAKE_INSTALL_PREFIX}/xerces
)

SET( XERCES_DIR ${CMAKE_INSTALL_PREFIX}/xerces CACHE PATH "XERCES install dir" FORCE )
