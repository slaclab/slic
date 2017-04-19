IF( NOT Geant4_VERSION )
    SET( Geant4_VERSION "v10.3.1" )
ENDIF()

IF ( Geant4_VERSION EQUAL "master" )
    MESSAGE( FATAL "Installing Geant4 from github master is not allowed." )
ENDIF()

EXTERNALPROJECT_ADD(
    Geant4

    GIT_REPOSITORY "https://github.com/Geant4/geant4"
    GIT_TAG ${GEANT4_VERSION}
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""

    SOURCE_DIR "${CMAKE_BINARY_DIR}/geant4"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/geant4 -DGEANT4_INSTALL_DATA=ON
    
    BUILD_COMMAND make -j4
)

STRING( REPLACE "v" "" Geant4_VERSION_NUMBER ${Geant4_VERSION} )
SET( Geant4_DIR ${CMAKE_INSTALL_PREFIX}/geant4/lib64/Geant4-${Geant4_VERSION_NUMBER} CACHE PATH "Geant4 install dir" FORCE )
