cmake_minimum_required(VERSION 3.12)

project(
  plugins
  VERSION 1.0
  DESCRIPTION "User action plugins." 
  LANGUAGES CXX)

file(GLOB SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/[a-zA-z]*.cc)
add_library(slicPlugins SHARED ${SRC_FILES})
target_include_directories(slicPlugins 
                           PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(slicPlugins 
                      PUBLIC ${Geant4_LIBRARIES})

install(TARGETS slicPlugins DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
