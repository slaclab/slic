##########################################################
# CMake configuration for SLIC application.              #
#                                                        #
# @author Jeremy McCormick <jeremym@slac.stanford.edu>   #
#                                                        #
##########################################################

# minimum cmake version
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

# project name
PROJECT( SLIC )

# project version
SET( SLIC_VERSION_MAJOR 6 )
SET( SLIC_VERSION_MINOR 0 )
SET( SLIC_VERSION_PATCH 0 )

INCLUDE( ExternalProject )

IF( NOT GLOBAL_INSTALL_DIR )
    SET( GLOBAL_INSTALL_DIR "$ENV{HOME}/slic" )
    MESSAGE( "GLOBAL_INSTALL_DIR='${GLOBAL_INSTALL_DIR}'" )
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

# set executable target
ADD_EXECUTABLE( slic ${library_sources} slic.cc )

# dependency list in build order
SET( DEPENDENCIES XERCES Geant4 LCIO HEPPDT GDML LCDD )
FOREACH( dependency ${DEPENDENCIES} ) 
    MESSAGE( STATUS "Finding ${dependency}" )
    IF( ${dependency}_DIR )
        FIND_PACKAGE( ${dependency} REQUIRED ${${dependency}_OPTIONS} )
        MESSAGE( STATUS "Found ${dependency} at: ${${dependency}_DIR}" )
    ELSE()
        MESSAGE( STATUS "${dependency} will be installed as external project." )
        INCLUDE( Install${dependency} )
        ADD_DEPENDENCIES( slic ${dependency} )
    ENDIF()
ENDFOREACH()

# find Xerces C++
#find_package ( XERCES REQUIRED )
#MESSAGE( STATUS "Found Xerces: ${XERCES_DIR}" )

# find LCIO
#if(NOT LCIO_DIR)
#    MESSAGE( STATUS "LCIO will be installed as external project." )
#    INCLUDE(InstallLCIO)
#    ADD_DEPENDENCIES(slic LCIO)
#ELSE()
#    FIND_PACKAGE( LCIO REQUIRED )
#    MESSAGE( STATUS "Found LCIO: ${LCIO_ROOT}" )
#ENDIF()

# find Geant4
#FIND_PACKAGE(Geant4 REQUIRED vis_all)
#MESSAGE( STATUS "Found Geant4: ${Geant4_DIR}" )
#MESSAGE( STATUS "Geant4 version: ${Geant4_VERSION}" )
#IF(Geant4_VERSION_MAJOR LESS 10)
#    MESSAGE(FATAL "Geant4 version ${Geant4_VERSION} is too old.")
#ENDIF()

# find GDML
#FIND_PACKAGE( GDML REQUIRED )
#MESSAGE( STATUS "Found GDML: ${GDML_DIR}" )

# install LCDD
#IF(NOT LCDD_DIR)
#    MESSAGE( STATUS "LCDD will be installed as external project." )
#    INCLUDE(InstallLCDD)
#    ADD_DEPENDENCIES( slic LCDD )
#ELSEIF()
#    FIND_PACKAGE( LCDD REQUIRED )
#    MESSAGE( STATUS "Found LCDD: ${LCDD_DIR}" )
#ENDIF()

# find HepPDT
#FIND_PACKAGE( HEPPDT REQUIRED )
#MESSAGE( STATUS "Found HEPPDT: ${HEPPDT_DIR}" )

# configure files
CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/include/PackageInfo.hh.in ${PROJECT_SOURCE_DIR}/include/PackageInfo.hh)
CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/include/Geant4VersionInfo.hh.in ${PROJECT_SOURCE_DIR}/include/Geant4VersionInfo.hh )
CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/scripts/create_slic_tarball.sh.in ${PROJECT_SOURCE_DIR}/scripts/create_slic_tarball.sh )

# additional compilation flags for Geant4
ADD_DEFINITIONS( ${Geant4_DEFINITIONS} )

# turn on compilation flag for enabling log statements that are defined within preprocessor blocks
IF ( ENABLE_SLIC_LOG STREQUAL "ON" )
    MESSAGE( STATUS "Enabling slic log system" )
    ADD_DEFINITIONS( -DSLIC_LOG )
ENDIF()

# local includes
SET( SLIC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include )
INCLUDE_DIRECTORIES( ${SLIC_INCLUDE_DIR} )

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

# configure and output documentation using doxygen
OPTION( INSTALL_DOC "Set to OFF to skip build/install documentation" OFF )
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
    MESSAGE( STATUS "Configuring doxygen input file" )
    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/config/doxygen/slic_doxygen.cfg.in ${PROJECT_SOURCE_DIR}/config/doxygen/slic_doxygen.cfg )

    # add doxygen documentation to install step
    INSTALL( CODE "EXECUTE_PROCESS( COMMAND doxygen ${PROJECT_SOURCE_DIR}/config/doxygen/slic_doxygen.cfg)" )

ELSE()
    # message that documentation is off for this build
    MESSAGE( STATUS "INSTALL_DOC is set to OFF.  Documentation will not be created." )
ENDIF()

# install
INSTALL( TARGETS slic slic DESTINATION bin )

# install env setup script
CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/scripts/slic-env.sh.in ${CMAKE_CURRENT_BINARY_DIR}/slic-env.sh )
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/slic-env.sh DESTINATION bin 
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
