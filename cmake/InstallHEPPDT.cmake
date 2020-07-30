EXTERNALPROJECT_ADD(
    HEPPDT

    GIT_REPOSITORY "https://github.com/slaclab/heppdt"
    GIT_TAG "master"

    SOURCE_DIR "${CMAKE_BINARY_DIR}/heppdt"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/heppdt/configure" --prefix=${DEPENDENCY_INSTALL_DIR}/heppdt
)

SET( HEPPDT_DIR ${DEPENDENCY_INSTALL_DIR}/heppdt CACHE PATH "HEPPDT install dir" FORCE )
