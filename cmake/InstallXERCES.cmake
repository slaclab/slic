EXTERNALPROJECT_ADD(
    
    XERCES
    
    URL "http://apache.cs.utah.edu//xerces/c/3/sources/xerces-c-3.1.4.tar.gz"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/extdep/xerces"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/extdep/xerces/configure" --enable-shared --disable-static --prefix=${GLOBAL_INSTALL_DIR}/xerces
)

SET( XERCES_DIR ${GLOBAL_INSTALL_DIR}/xerces CACHE PATH "XERCES install dir" FORCE )

#MESSAGE( "install set XERCES_DIR='${XERCES_DIR}'" )

# TODO: move to UseXerces macro
SET( XERCES_INCLUDE_DIR ${XERCES_DIR}/include )
SET( XERCES_LIBRARY ${XERCES_DIR}/lib/libxerces-c.so )
