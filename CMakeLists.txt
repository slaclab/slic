##########################################################
# CMake configuration for SLIC application.              #
#                                                        #
# @author Jeremy McCormick <jeremym@slac.stanford.edu>   #
#                                                        #
##########################################################

cmake_minimum_required(VERSION 3.0)

project(SLIC VERSION 6.0.1)

#message(STATUS "Configuring SLIC version: ${SLIC_VERSION}")

if(CMAKE_CXX_COMPILER_VERSION LESS 4.8)
    message(FATAL_ERROR "The GCC version is too old (at least 4.8 is required).")
ENDIF()

#set(CMAKE_VERBOSE_MAKEFILE TRUE)

# add path containing CMake macros for this project
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# include CMake macros
include(ExternalProject)
include(FindPackageHandleStandardArgs)

# external dependencies that are installed are added as dependencies of this target
add_custom_target(dependencies)

# find or build external dependencies
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
file(GLOB_RECURSE plugin_sources ${PROJECT_SOURCE_DIR}/plugins/*.cc)
add_library(slicPlugins SHARED ${plugin_sources})
target_link_libraries(slicPlugins ${Geant4_LIBRARIES})
add_dependencies(slicPlugins dependencies) # make sure deps are built first
install(TARGETS slicPlugins DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

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

message(STATUS "SLIC will be installed to: ${CMAKE_INSTALL_PREFIX}")

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
