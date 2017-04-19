IF( NOT LCIO_VERSION )
    SET( LCIO_VERSION "master" )
ENDIF()

EXTERNALPROJECT_ADD(
    LCIO

    GIT_REPOSITORY "https://github.com/iLCSoft/LCIO"
    GIT_TAG "${LCIO_VERSION}"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""

    SOURCE_DIR "${CMAKE_BINARY_DIR}/extdep/lcio"
    CMAKE_ARGS -DINSTALL_DOC=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/lcio
    
    BUILD_COMMAND make -j4
)

SET( LCIO_DIR ${CMAKE_INSTALL_PREFIX}/lcio CACHE PATH "LCIO install dir" FORCE )
