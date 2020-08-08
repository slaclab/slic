message(STATUS "Checking for Geant4")
find_package(Geant4 QUIET COMPONENTS vis_all ui_all)
if(NOT Geant4_FOUND)
    message(STATUS "Geant4 was not found and will be installed")
    set(Geant4_VERSION 10.6.1 CACHE STRING "Geant4 version" FORCE)
    set(Geant4_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/Geant4})
    add_dependencies(dependencies Geant4)
    externalproject_add(
        Geant4
        GIT_REPOSITORY   "https://github.com/Geant4/geant4"
        GIT_TAG          v${Geant4_VERSION}
        GIT_SHALLOW      ON
        SOURCE_DIR       ${CMAKE_BINARY_DIR}/Geant4
        CMAKE_ARGS       -DCMAKE_INSTALL_PREFIX=${Geant4_INSTALL_DIR} -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_SYSTEM_EXPAT=OFF -DGEANT4_INSTALL_EXAMPLES=OFF
        BUILD_COMMAND    ${CMAKE_MAKE_PROGRAM} -j4
        UPDATE_COMMAND   ""
        BUILD_IN_SOURCE  ON
        BUILD_ALWAYS     ON
        EXCLUDE_FROM_ALL ON
    )
    # -DGEANT4_USE_OPENGL_X11=${GEANT4_USE_OPENGL_X11} -DGEANT4_USE_QT=${GEANT4_USE_QT}
    set(Geant4_DIR ${Geant4_INSTALL_DIR}/lib/Geant4-${Geant4_VERSION} CACHE PATH "Geant4 install dir" FORCE)
    set(Geant4_INCLUDE_DIRS ${Geant4_INSTALL_DIR}/include/Geant4 CACHE PATH "Geant4 include dirs" FORCE)
    set(Geant4_LIBRARIES G4analysis
                         G4clhep
                         G4digits_hits
                         G4error_propagation
                         G4event
                         G4expat
                         G4FR
                         G4geometry
                         G4global
                         G4GMocren
                         G4graphics_reps
                         G4intercoms
                         G4interfaces
                         G4materials
                         G4modeling
                         G4parmodels
                         G4particles
                         G4persistency
                         G4physicslists
                         G4processes
                         G4RayTracer
                         G4readout
                         G4run
                         G4track
                         G4tracking
                         G4Tree
                         G4visHepRep
                         G4visXXX
                         G4vis_management
                         G4VRML
                         G4z)
else()
    message(STATUS "Geant4 was found at: ${Geant4_DIR}")
    #message(STATUS "Geant4 libraries: ${Geant4_LIBRARIES}")
    #message(STATUS "Geant4 include dirs: ${Geant4_INCLUDE_DIRS}")
endif()

include_directories(${Geant4_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Geant4_LIBRARIES})
