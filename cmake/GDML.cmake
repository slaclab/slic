message(STATUS "Checking for GDML")
find_package(GDML QUIET)
if(NOT GDML_FOUND)
    message(STATUS "GDML was not found and will be installed")
    add_dependencies(dependencies LCIO)
    set(GDML_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/GDML)
    externalproject_add(
        GDML
        DEPENDS XercesC Geant4
        GIT_REPOSITORY  "https://github.com/slaclab/gdml"
        GIT_TAG         "master"
        GIT_SHALLOW     ON
        UPDATE_COMMAND  ""
        PATCH_COMMAND   ""
        SOURCE_DIR      "${CMAKE_BINARY_DIR}/gdml"
        CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${GDML_INSTALL_DIR} 
                        -DXercesC_LIBRARY=${XercesC_LIBRARY} -DXercesC_INCLUDE_DIR=${XercesC_INCLUDE_DIR} 
                        -DGeant4_DIR=${Geant4_DIR}
                        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CC_COMPILER=${CMAKE_CC_COMPILER}
        BUILD_COMMAND   ${CMAKE_MAKE_PROGRAM} -j4
    )
    set(GDML_DIR ${GDML_INSTALL_DIR} CACHE PATH "GDML root dir" FORCE)
    set(GDML_LIBRARY ${GDML_INSTALL_DIR}/lib/libgdml${CMAKE_SHARED_LIBRARY_SUFFIX} CACHE FILEPATH "GDML library path" FORCE)
    set(GDML_INCLUDE_DIR ${GDML_INSTALL_DIR}/include CACHE PATH "GDML inclued dir" FORCE)
else()
    add_custom_target(GDML) # dummy target
    message(STATUS "Found GDML lib: ${GDML_LIBRARY}")
endif()

include_directories(${GDML_INCLUDE_DIR})
set(LIBS ${LIBS} ${GDML_LIBRARY})
