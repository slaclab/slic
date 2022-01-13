# Enable vis and UI libraries (off by default since batch mode is most typical)
option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" OFF)

# Set the Geant4 release version if not passed as an option
if(NOT DEFINED WITH_GEANT4_VERSION)
    set(WITH_GEANT4_VERSION "10.6.1")
endif()

message(STATUS "Checking for Geant4")
if(WITH_GEANT4_UIVIS)
    find_package(Geant4 QUIET COMPONENTS ui_all vis_all)
else()
    find_package(Geant4 QUIET)
endif()

if(NOT Geant4_FOUND)
    message(STATUS "Geant4 was not found and will be installed with version: ${WITH_GEANT4_VERSION}")
    message(STATUS "Geant4 UI and vis components enabled: ${WITH_GEANT4_UIVIS}")

    set(Geant4_VERSION ${WITH_GEANT4_VERSION} CACHE STRING "Geant4 version" FORCE)
    set(Geant4_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/Geant4)
    add_dependencies(dependencies Geant4)
    externalproject_add(
        Geant4
        DEPENDS          XercesC
        GIT_REPOSITORY   "https://github.com/Geant4/geant4"
        GIT_TAG          v${Geant4_VERSION}
        GIT_SHALLOW      ON
        SOURCE_DIR       ${CMAKE_BINARY_DIR}/Geant4
        CMAKE_ARGS       -DCMAKE_INSTALL_PREFIX=${Geant4_INSTALL_DIR} -DGEANT4_INSTALL_DATA=ON 
                         -DGEANT4_USE_GDML=ON -DGEANT4_USE_SYSTEM_EXPAT=OFF -DGEANT4_INSTALL_EXAMPLES=OFF 
                         -DGEANT4_USE_OPENGL_X11=${WITH_GEANT4_UIVIS} -DGEANT4_USE_QT=${WITH_GEANT4_UIVIS}
                         -DXERCESC_ROOT_DIR=${XercesC_INSTALL_DIR} 
                         -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CC_COMPILER=${CMAKE_CC_COMPILER}
                         -DCMAKE_INSTALL_LIBDIR=${Geant4_INSTALL_DIR}/lib

        BUILD_COMMAND    ${CMAKE_MAKE_PROGRAM} -j4
        UPDATE_COMMAND   ""
    )
    # vis options left out for now
    set(Geant4_DIR ${Geant4_INSTALL_DIR}/lib/Geant4-${Geant4_VERSION} CACHE PATH "Geant4 install dir" FORCE)
    set(Geant4_INCLUDE_DIRS ${Geant4_INSTALL_DIR}/include/Geant4 CACHE PATH "Geant4 include dirs" FORCE)
    # list of Geant4 libraries (don't know another way to do this since we need this info before G4 is actually built & installed)
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
                         G4zlib)
else()
    add_custom_target(Geant4) # dummy target
    message(STATUS "Geant4 was found at: ${Geant4_DIR}")
    message(STATUS "Geant4 libraries: ${Geant4_LIBRARIES}")
    message(STATUS "Geant4 include dirs: ${Geant4_INCLUDE_DIRS}")
endif()

include_directories(${Geant4_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Geant4_LIBRARIES})
