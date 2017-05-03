EXTERNALPROJECT_ADD(
    HEPPDT

    URL "http://lcgapp.cern.ch/project/simu/HepPDT/download/HepPDT-3.04.01.tar.gz"

    SOURCE_DIR "${CMAKE_BINARY_DIR}/heppdt"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/heppdt/configure" --prefix=${DEPENDENCY_INSTALL_DIR}/heppdt
)

SET( HEPPDT_DIR ${DEPENDENCY_INSTALL_DIR}/heppdt CACHE PATH "HEPPDT install dir" FORCE )
