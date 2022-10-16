message(STATUS "Checking for LCDD")
find_package(LCDD QUIET)
if(NOT LCDD_FOUND)
    message(STATUS "LCDD was not found and will be installed")
    set(LCDD_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/LCDD)
    externalproject_add(
        LCDD
        DEPENDS        XercesC Geant4 GDML
        GIT_REPOSITORY "https://github.com/slaclab/lcdd"
        GIT_TAG        "master"
        GIT_SHALLOW    ON
        UPDATE_COMMAND ""
        PATCH_COMMAND  ""
        SOURCE_DIR     "${CMAKE_BINARY_DIR}/lcdd"
        CMAKE_ARGS     -DINSTALL_DOC=OFF -DCMAKE_INSTALL_PREFIX=${LCDD_INSTALL_DIR} 
                       -DGDML_DIR=${GDML_DIR} 
                       -DXercesC_LIBRARY=${XercesC_LIBRARY} -DXercesC_INCLUDE_DIR=${XercesC_INCLUDE_DIR} 
                       -DGeant4_DIR=${Geant4_DIR}
                       -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CC_COMPILER=${CMAKE_CC_COMPILER}
        BUILD_COMMAND  ${CMAKE_MAKE_PROGRAM} -j4
    )
    set(LCDD_DIR ${LCDD_INSTALL_DIR} CACHE PATH "LCDD root dir" FORCE)
    set(LCDD_LIBRARY ${LCDD_INSTALL_DIR}/lib/liblcdd${CMAKE_SHARED_LIBRARY_SUFFIX} CACHE FILEPATH "LCDD library path" FORCE)
    set(LCDD_INCLUDE_DIR ${LCDD_INSTALL_DIR}/include CACHE PATH "LCDD include dir" FORCE)
else()
    add_custom_target(LCDD) # dummy target
    message(STATUS "LCDD library found at: ${LCDD_LIBRARY}")
endif()

add_dependencies(dependencies LCDD)
include_directories(${LCDD_INCLUDE_DIR})
set(LIBS ${LIBS} ${LCDD_LIBRARY})
