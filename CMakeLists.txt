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
SET( SLIC_VERSION_MAJOR 5 )
SET( SLIC_VERSION_MINOR 0 )
SET( SLIC_VERSION_PATCH 1 )

# option for enabling and disabling the logging system
OPTION( ENABLE_SLIC_LOG "enable slic logging system" ON )

# option to dump CMake variables after cmake is executed
OPTION( PRINT_CMAKE_VARIABLES "print CMake environment" OFF )

# find ilcutil
FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES REQUIRED )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )
INCLUDE( ExternalProject )

# add path containing CMake macros for this project
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

# macro for generating configuration output
INCLUDE( MacroGeneratePackageConfigFiles )

# find Xerces C++
find_package ( XERCES REQUIRED )
IF( NOT XERCES_FOUND )
	MESSAGE( FATAL_ERROR "Xerces was not found!" )
ENDIF()

# find LCIO
FIND_PACKAGE( LCIO REQUIRED )
IF ( NOT LCIO_FOUND )
    MESSAGE( FATAL_ERROR "LCIO was not found!" )
ELSE()
    MESSAGE( STATUS "Found LCIO: ${LCIO_ROOT}" )
ENDIF()

# find Geant4
find_package(Geant4 REQUIRED)
IF ( NOT Geant4_FOUND )
    MESSAGE( FATAL_ERROR "Geant4 was not found!  Define Geant4_DIR when calling cmake." )
ELSE()
    MESSAGE( STATUS "Found Geant4: ${Geant4_DIR}" )
ENDIF()

MESSAGE( STATUS "Geant4_VERSION=${Geant4_VERSION}" )

# enable visualization
find_package(Geant4 REQUIRED vis_all)

# find GDML
FIND_PACKAGE( GDML REQUIRED )
IF ( NOT GDML_FOUND )
    MESSAGE( FATAL_ERROR "Geant4 was not found!" )
ELSE()
    MESSAGE( STATUS "Found GDML: ${GDML_DIR}" )
ENDIF()

# find LCDD
FIND_PACKAGE( LCDD REQUIRED )
IF ( NOT LCDD_FOUND )
    MESSAGE( FATAL_ERROR "LCDD was not found!" )
ELSE()
    MESSAGE( STATUS "Found LCDD: ${LCDD_DIR}" )
ENDIF()

# find LCDD
FIND_PACKAGE( HEPPDT REQUIRED )
IF ( NOT HEPPDT_FOUND )
    MESSAGE( FATAL_ERROR "HEPPDT was not found!" )
ELSE()
    MESSAGE( STATUS "Found HEPPDT: ${HEPPDT_DIR}" )
ENDIF()

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

# debug flags
SET ( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -DDEBUG=1 -pg -ggdb" )
SET ( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3" )

# local includes
SET( SLIC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include )
INCLUDE_DIRECTORIES( ${SLIC_INCLUDE_DIR} )

# include directories
INCLUDE_DIRECTORIES( ${XERCES_INCLUDE_DIR} ${LCIO_INCLUDE_DIRS} ${Geant4_INCLUDE_DIRS} ${GDML_INCLUDE_DIRS} ${LCDD_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIR} )

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

# get list of sources
FILE( GLOB_RECURSE library_sources ${PROJECT_SOURCE_DIR}/src/*.cc )

# executable
ADD_EXECUTABLE( slic ${library_sources} slic.cc )

# configure and output documentation using doxygen
OPTION( INSTALL_DOC "Set to OFF to skip build/install documentation" ON )
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

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
