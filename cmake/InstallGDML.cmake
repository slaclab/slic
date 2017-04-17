IF( NOT GDML_VERSION )
    SET( GDML_VERSION "master" )
ENDIF()

EXTERNALPROJECT_ADD(

    GDML

    DEPENDS XERCES Geant4 

    GIT_REPOSITORY "https://github.com/slaclab/gdml"
    GIT_TAG "${GDML_VERSION}"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""

    SOURCE_DIR "${CMAKE_BINARY_DIR}/extdep/gdml"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${GLOBAL_INSTALL_DIR}/gdml -DXERCES_DIR=${XERCES_DIR} -DGeant4_DIR=${Geant4_DIR}
    
    BUILD_COMMAND make -j4
)

SET( GDML_DIR ${GLOBAL_INSTALL_DIR}/gdml CACHE PATH "GDML install dir" FORCE )

MESSAGE( "install set GDML_DIR='${GDML_DIR}'" )

# TODO: move to UseGDML macro
SET( GDML_INCLUDE_DIR ${GDML_DIR}/include )
SET( GDML_LIBRARY_DIR ${GDML_DIR}/lib )
SET( GDML_LIBRARY gdml )
