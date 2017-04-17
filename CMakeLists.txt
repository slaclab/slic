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
SET( SLIC_VERSION_PATCH 0 )

INCLUDE( ExternalProject )

IF( NOT GLOBAL_INSTALL_DIR )
    SET( GLOBAL_INSTALL_DIR "$ENV{HOME}/slic" )
    MESSAGE( STATUS "GLOBAL_INSTALL_DIR='${GLOBAL_INSTALL_DIR}'" )
ENDIF()

# turn on compilation flag for enabling log statements that are defined within preprocessor blocks
IF ( ENABLE_SLIC_LOG STREQUAL "ON" )
    MESSAGE( STATUS "Enabling slic log system" )
    ADD_DEFINITIONS( -DSLIC_LOG )
ENDIF()

SET( CMAKE_INSTALL_PREFIX ${GLOBAL_INSTALL_DIR}/slic )

# option for enabling and disabling the logging system
OPTION( ENABLE_SLIC_LOG "enable slic logging system" ON )

# option to dump CMake variables after cmake is executed
OPTION( PRINT_CMAKE_VARIABLES "print CMake environment" OFF )

# add path containing CMake macros for this project
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

# get list of sources
FILE( GLOB_RECURSE library_sources ${PROJECT_SOURCE_DIR}/src/*.cc )

# dependency list in build order
SET( DEPENDENCIES XERCES Geant4 LCIO HEPPDT GDML LCDD )

ADD_CUSTOM_TARGET( extdeps )
FOREACH( dependency ${DEPENDENCIES} )
    MESSAGE( "${dependency}_DIR='${${dependency}_DIR}'" )
    IF ( ${dependency}_DIR )
        FIND_PACKAGE( ${dependency} QUIET )
        IF ( ${dependency}_FOUND )
            MESSAGE( STATUS "${dependency} was found at: '${${dependency}_DIR}'" )
            IF( NOT TARGET ${dependency} )
                ADD_CUSTOM_TARGET( ${dependency} )
            ENDIF()
        ELSE()
            MESSAGE( FATAL_ERROR "You specified ${dependency}_DIR but this does not appear to point to a valid install of ${dependency}." )
        ENDIF()
    ELSE()
        MESSAGE( STATUS "${dependency} was not found and will be installed as external project." )
        INCLUDE( Install${dependency} )
        SET( DEPENDENCIES_NOTFOUND "YES" FORCE )
    ENDIF()
    ADD_DEPENDENCIES( extdeps ${dependency} )
ENDFOREACH()

IF ( DEPENDENCIES_NOTFOUND )
    MESSAGE( "Some dependencies were not found.  Run 'make' to build them or rerun cmake with their directory arguments (e.g. Geant4_DIR)." ) 
ELSE()

    MESSAGE( "All dependencies were found.  Building SLIC executable ..." )

    # set executable target
    ADD_EXECUTABLE( slic ${library_sources} slic.cc )

    ADD_DEPENDENCIES( slic extdeps )

    # local includes
    SET( SLIC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include )
    INCLUDE_DIRECTORIES( ${SLIC_INCLUDE_DIR} )

    # configure files
    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/include/PackageInfo.hh.in ${PROJECT_SOURCE_DIR}/include/PackageInfo.hh)
    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/include/Geant4VersionInfo.hh.in ${PROJECT_SOURCE_DIR}/include/Geant4VersionInfo.hh )
    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/scripts/create_slic_tarball.sh.in ${PROJECT_SOURCE_DIR}/scripts/create_slic_tarball.sh )

    # additional compilation flags for Geant4
    ADD_DEFINITIONS( ${Geant4_DEFINITIONS} )

    # include directories
    INCLUDE_DIRECTORIES( ${XERCES_INCLUDE_DIR} ${LCIO_INCLUDE_DIRS} ${Geant4_INCLUDE_DIRS} ${GDML_INCLUDE_DIR} ${LCDD_INCLUDE_DIR} ${HEPPDT_INCLUDE_DIR} )

    # add GDML link directory
    LINK_DIRECTORIES( ${GDML_LIBRARY_DIR} )
    LINK_DIRECTORIES( ${LCDD_LIBRARY_DIR} )
    LINK_DIRECTORIES( ${LCIO_LIBRARY_DIRS} )

    # libraries
    LINK_LIBRARIES( ${XERCES_LIBRARY} )
    LINK_LIBRARIES( ${Geant4_LIBRARIES} )
    LINK_LIBRARIES( ${GDML_LIBRARY} )
    LINK_LIBRARIES( ${LCDD_LIBRARY} )
    LINK_LIBRARIES( ${HEPPDT_LIBRARIES} )
    LINK_LIBRARIES( ${LCIO_LIBRARIES} )

    # install
    INSTALL( TARGETS slic slic DESTINATION bin )

    # install env setup script
    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/scripts/slic-env.sh.in ${CMAKE_CURRENT_BINARY_DIR}/slic-env.sh )
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/slic-env.sh DESTINATION bin 
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
ENDIF()

# configure and output documentation using doxygen
#OPTION( INSTALL_DOC "Set to OFF to skip build/install documentation" OFF )
#IF ( INSTALL_DOC )

    # find doxygen
#    FIND_PROGRAM( DOXYGEN_EXECUTABLE doxygen ${PATH} )
#    IF ( DOXYGEN_EXECUTABLE-NOTFOUND )
#        MESSAGE( FATAL_ERROR "The doxygen executable was not found." )
#    ENDIF()

    # find dot
#    FIND_PROGRAM( DOT_EXECUTABLE dot ${PATH} )
#    IF ( DOT_EXECUTABLE-NOTFOUND )
#        MESSAGE( FATAL_ERROR "The dot executable was not found." )
#    ENDIF()

    # configure doxygen output
#    MESSAGE( STATUS "Configuring doxygen input file" )
#    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/config/doxygen/slic_doxygen.cfg.in ${PROJECT_SOURCE_DIR}/config/doxygen/slic_doxygen.cfg )

    # add doxygen documentation to install step
#    INSTALL( CODE "EXECUTE_PROCESS( COMMAND doxygen ${PROJECT_SOURCE_DIR}/config/doxygen/slic_doxygen.cfg)" )

#ELSE()
    # message that documentation is off for this build
#    MESSAGE( STATUS "INSTALL_DOC is set to OFF.  Documentation will not be created." )
#ENDIF()

