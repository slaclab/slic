message(STATUS "Checking for HepPDT")
find_package(HepPDT QUIET)
if(NOT HepPDT_FOUND)
    message(STATUS "HepPDT was not found and will be installed")
    add_dependencies(dependencies HepPDT)
    set(HepPDT_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/HepPDT)
    externalproject_add(
        HepPDT
        GIT_REPOSITORY    "https://github.com/slaclab/heppdt"
        GIT_TAG           "master"
        SOURCE_DIR        "${CMAKE_BINARY_DIR}/HepPDT"
        UPDATE_COMMAND    ""
        PATCH_COMMAND     ""
        CONFIGURE_COMMAND ./configure --prefix=${HepPDT_INSTALL_DIR}
        BUILD_ALWAYS      ON
        BUILD_IN_SOURCE   ON
        EXCLUDE_FROM_ALL  ON
    )
    set(HepPDT_DIR ${HepPDT_INSTALL_DIR})
    set(HepPDT_LIBRARIES ${HepPDT_INSTALL_DIR}/lib/libHepPDT.so ${HepPDT_INSTALL_DIR}/lib/libHepPID.so)
    set(HepPDT_INCLUDE_DIR ${HepPDT_INSTALL_DIR}/include)
else()
    message(STATUS "HepPDT libraries found at: ${HepPDT_LIBRARIES}")
endif()

include_directories(${HepPDT_INCLUDE_DIR})
set(LIBS ${LIBS} ${HepPDT_LIBRARIES})