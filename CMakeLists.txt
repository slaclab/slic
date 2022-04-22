##########################################################
# CMake configuration for SLIC application.              #
#                                                        #
# @author Jeremy McCormick <jeremym@slac.stanford.edu>   #
#                                                        #
##########################################################

# minimum version of CMake, which is determined by Geant4's requirements
cmake_minimum_required(VERSION 3.8)

project(SLIC VERSION 6.1.0)

# enable in source build since default is not writable from a non-root account
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "..." FORCE)
endif()

message(STATUS "SLIC will be installed to: ${CMAKE_INSTALL_PREFIX}")

# minimum gcc version
if(CMAKE_CXX_COMPILER_VERSION LESS 4.8)
    message(FATAL_ERROR "The GCC version is too old (at least 4.8 is required).")
ENDIF()

if(APPLE)
    # use, i.e. don't skip the full RPATH for the build tree
    set(CMAKE_SKIP_BUILD_RPATH  FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

    set(CMAKE_INSTALL_RPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    # the RPATH to be used when installing, but only if it's not a system directory
    list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" isSystemDir)
    IF("${isSystemDir}" STREQUAL "-1")
        set(CMAKE_INSTALL_RPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
        message("CMAKE_INSTALL_RPATH set to ${CMAKE_INSTALL_RPATH} ")

    ENDIF("${isSystemDir}" STREQUAL "-1")
endif(APPLE)

# add path containing CMake macros for this project
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# include CMake macros
include(ExternalProject)
include(FindPackageHandleStandardArgs)

# external dependencies are added as dependencies of this target
add_custom_target(dependencies)

# include macros to find or build external dependencies
include(XercesC)
include(Geant4)
include(HepPDT)
include(LCIO)
include(GDML)
include(LCDD)

# get list of sources
file(GLOB_RECURSE library_sources ${PROJECT_SOURCE_DIR}/src/*.cc)

# add Geant4 library dir
link_directories(${Geant4_DIR}/..)

# set executable target
add_executable(slic ${library_sources} slic.cc)

# set local include dir
set(SLIC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
include_directories(${SLIC_INCLUDE_DIR})

# build user plugin library
add_subdirectory(plugins)

# make slic depend on any external projects that were installed
# (could be an empty target if all packages were found)
add_dependencies(slic dependencies)

# set library dependencies
target_link_libraries(slic ${LIBS} dl)

# option for SLIC log messages
option(WITH_SLIC_LOGGING "Enable SLIC logging system" ON)
if(WITH_SLIC_LOGGING STREQUAL "ON")
    add_definitions(-DSLIC_LOG)
endif()
message(STATUS "SLIC logging: ${WITH_SLIC_LOGGING}")

# install the binary
install(TARGETS slic slic DESTINATION bin)

# install environment setup script
configure_file( ${PROJECT_SOURCE_DIR}/scripts/slic-env.sh.in ${CMAKE_CURRENT_BINARY_DIR}/slic-env.sh)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/slic-env.sh DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# install particle data table
install(FILES data/particle.tbl DESTINATION share PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

# configure the header which has package info
configure_file(${PROJECT_SOURCE_DIR}/include/PackageInfo.hh.in ${PROJECT_SOURCE_DIR}/include/PackageInfo.hh)

# debug print link libraries
message(DEBUG "libraries")
foreach(lib ${LIBS})
    message(DEBUG "    ${lib}")
endforeach()

# debug print include dirs
message(DEBUG "include dirs")
get_property(include_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(include_dir ${include_dirs})
    message(DEBUG "    ${include_dir}")
endforeach()

# barf out CMake variables at trace level
get_cmake_property(varnames VARIABLES)
message(TRACE "variables")
foreach(varname ${varnames})
    message(TRACE "${varname}=${${varname}}")
endforeach()
