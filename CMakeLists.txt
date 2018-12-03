##########################################################
# CMake configuration for SLIC application.              #
#                                                        #
# @author Jeremy McCormick <jeremym@slac.stanford.edu>   #
#                                                        #
##########################################################

PROJECT( SLIC )

CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )

IF( CMAKE_CXX_COMPILER_VERSION LESS 4.8 )
    MESSAGE( FATAL_ERROR "The gcc compiler version '${CMAKE_CXX_COMPILER_VERSION}' is too old (at least 4.8 required)." )
ENDIF()

SET( SLIC_VERSION_MAJOR 6 )
SET( SLIC_VERSION_MINOR 0 )
SET( SLIC_VERSION_PATCH 1 )

INCLUDE( ExternalProject )

MESSAGE( STATUS "SLIC install dir set to: ${CMAKE_INSTALL_PREFIX}" )

# option for log messages
OPTION( ENABLE_SLIC_LOG "Enable slic logging system" ON )
IF( ENABLE_SLIC_LOG STREQUAL "ON" )
    ADD_DEFINITIONS( -DSLIC_LOG )
ENDIF()
MESSAGE( STATUS "Enable slic log system: ${ENABLE_SLIC_LOG}" )

# option to dump CMake variables after cmake is executed
OPTION( PRINT_CMAKE_VARIABLES "Print CMake environment" OFF )

# enable build and installation of external dependencies
OPTION( INSTALL_DEPENDENCIES "Install external dependencies automatically" ON )

# enable Geant4 visualization and UI components
option( WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON )

# add path containing CMake macros for this project
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

# get list of sources
FILE( GLOB_RECURSE library_sources ${PROJECT_SOURCE_DIR}/src/*.cc )

# dependency list in build order
SET( DEPENDENCIES XERCES Geant4 GDML LCDD LCIO HEPPDT )

IF( NOT DEPENDENCY_INSTALL_DIR )
    SET( DEPENDENCY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/extdeps )
ENDIF()

ADD_CUSTOM_TARGET( extdeps )
FOREACH( dependency ${DEPENDENCIES} )
    MESSAGE( "Finding external dependency: ${dependency}" )
    FIND_PACKAGE( ${dependency} )
    IF ( ${dependency}_FOUND )
        MESSAGE( STATUS "${dependency} was found at: ${${dependency}_DIR}" )
        IF( NOT TARGET ${dependency} )
            ADD_CUSTOM_TARGET( ${dependency} )
        ENDIF()
        IF ( ${dependency} STREQUAL "Geant4" AND WITH_GEANT4_UIVIS )
            MESSAGE( "Finding Geant4 with vis and ui components enabled" )
            FIND_PACKAGE( Geant4 REQUIRED vis_all ui_all )
            MESSAGE( STATUS "The vis and UI components are enabled in Geant4." )
        ENDIF()
    ELSE()
        IF ( NOT INSTALL_DEPENDENCIES )
            MESSAGE( FATAL_ERROR "${dependency} was not found and dependency installation is disabled.\nUse the option '-DINSTALL_DEPENDENCIES=ON' to enable automatic dependency installation." )
        ELSE()
            MESSAGE( STATUS "${dependency} was not found and will be installed as an external project." )
            INCLUDE( Install${dependency} )
            SET( DEPENDENCIES_NOTFOUND "YES" FORCE )
        ENDIF()
    ENDIF()
    ADD_DEPENDENCIES( extdeps ${dependency} )
    #MESSAGE("  ${dependency}_DIR=${${dependency}_DIR}" )
    #MESSAGE("  ${dependency}_VERSION=${${dependency}_VERSION}" )
    #MESSAGE("  ${dependency}_FOUND=${${dependency}_FOUND}" )
ENDFOREACH()

IF ( DEPENDENCIES_NOTFOUND )
    MESSAGE( "Some dependencies were not found.  Run 'make' to build them, or rerun CMake and provide their location (e.g. Geant4_DIR etc.)." ) 
    MESSAGE( "Set '-DDEPENDENCY_INSTALL_DIR=/my/install/dir' to set the directory where external projects will be installed." )
    MESSAGE( "After building any missing dependencies you will need to rerun CMake to resolve them for the build." )
ELSE()

    MESSAGE( "All dependencies were found.  Run 'make' to build SLIC and 'make install' to install to: ${CMAKE_INSTALL_PREFIX}" )

    # build user plugin library
    FILE( GLOB_RECURSE plugin_sources ${PROJECT_SOURCE_DIR}/plugins/*.cc )
    ADD_LIBRARY( slicPlugins SHARED ${plugin_sources} )
    TARGET_LINK_LIBRARIES(slicPlugins ${Geant4_LIBRARIES} )
    INSTALL( TARGETS slicPlugins DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )

    # set executable target
    ADD_EXECUTABLE( slic ${library_sources} slic.cc )

    # make slic depend on its external deps
    ADD_DEPENDENCIES( slic extdeps slicPlugins )
    
    # local includes
    SET( SLIC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include )
    INCLUDE_DIRECTORIES( ${SLIC_INCLUDE_DIR} )

    # configure files
    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/include/PackageInfo.hh.in ${PROJECT_SOURCE_DIR}/include/PackageInfo.hh)

    # additional compilation flags for Geant4
    INCLUDE( ${Geant4_USE_FILE} )

    # include directories
    INCLUDE_DIRECTORIES( ${XERCES_INCLUDE_DIR} ${LCIO_INCLUDE_DIRS} ${Geant4_INCLUDE_DIRS} ${GDML_INCLUDE_DIR} ${LCDD_INCLUDE_DIR} ${HEPPDT_INCLUDE_DIR} )

    # check for LCStdHepRdrNew class in LCIO
    IF ( EXISTS "${LCIO_INCLUDE_DIRS}/UTIL/LCStdHepRdrNew.h" )
        ADD_DEFINITIONS( -DHAVE_STDHEPRDRNEW_H )
    ENDIF()

    # check if MCParticle implementation has endpoint momentum 
    SET( CMAKE_REQUIRED_INCLUDES ${LCIO_INCLUDE_DIRS} )
    INCLUDE(CheckCXXSourceCompiles)
    CHECK_CXX_SOURCE_COMPILES(
        "
        #include \"EVENT/MCParticle.h\"
        int main() {
            EVENT::MCParticle* p;
            p->getMomentumAtEndpoint();
            return 0;
        }
        " MCPARTICLE_HAS_MOMENTUMATENDPOINT
    )
    IF ( MCPARTICLE_HAS_MOMENTUMATENDPOINT )
        ADD_DEFINITIONS( -DMCPARTICLE_HAS_MOMENTUMATENDPOINT )
    ENDIF()

    # libraries
    TARGET_LINK_LIBRARIES( slic ${XERCES_LIBRARY} ${Geant4_LIBRARIES} ${GDML_LIBRARY} ${LCDD_LIBRARY} ${HEPPDT_LIBRARIES} ${LCIO_LIBRARIES} slicPlugins dl )

    # link directories
    LINK_DIRECTORIES( ${GDML_LIBRARY_DIR} ${LCDD_LIBRARY_DIR} ${LCIO_LIBRARY_DIRS} )

    # install
    INSTALL( TARGETS slic slic DESTINATION bin )

    # install env setup script
    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/scripts/slic-env.sh.in ${CMAKE_CURRENT_BINARY_DIR}/slic-env.sh )
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/slic-env.sh DESTINATION bin 
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

    # install particle data tbl
    INSTALL(FILES data/particle.tbl DESTINATION share PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
ENDIF()

# configure and output documentation using doxygen
OPTION( INSTALL_DOC "Build documentation using doxygen" OFF )
IF ( INSTALL_DOC )

    # find doxygen
    FIND_PROGRAM( DOXYGEN_EXECUTABLE doxygen ${PATH} )
    IF ( DOXYGEN_EXECUTABLE-NOTFOUND )
        MESSAGE( FATAL_ERROR "The doxygen executable was not found." )
    ENDIF()

    # find dot
    FIND_PROGRAM( DOT_EXECUTABLE dot ${PATH} )
    IF ( DOT_EXECUTABLE-NOTFOUND )
        MESSAGE( FATAL_ERROR "The dot executable was not found." )
    ENDIF()

    # configure doxygen output
    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/config/doxygen/slic_doxygen.cfg.in ${PROJECT_SOURCE_DIR}/config/doxygen/slic_doxygen.cfg )

    # add doxygen documentation to install step
    INSTALL( CODE "EXECUTE_PROCESS( COMMAND doxygen ${PROJECT_SOURCE_DIR}/config/doxygen/slic_doxygen.cfg)" )

ENDIF()
MESSAGE( STATUS "Install documentation using doxygen: ${INSTALL_DOC}" )

IF ( PRINT_CMAKE_VARIABLES )
    GET_CMAKE_PROPERTY(_variableNames VARIABLES)
    MESSAGE( "Dumping CMake variables ..." )
    FOREACH(_variableName ${_variableNames})
        MESSAGE(STATUS "${_variableName}=${${_variableName}}")
    ENDFOREACH()
ENDIF()
