message(STATUS "Checking for LCIO")
find_package(LCIO QUIET)
if(NOT LCIO_FOUND)
    message(STATUS "LCIO was not found and will be installed")
    set(LCIO_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/LCIO)
    externalproject_add(
        LCIO
        GIT_REPOSITORY   "https://github.com/iLCSoft/LCIO"
        GIT_TAG          "v02-07-05"
        GIT_SHALLOW      ON
        UPDATE_COMMAND   ""
        PATCH_COMMAND    ""
        SOURCE_DIR       ${CMAKE_BINARY_DIR}/LCIO
        CMAKE_ARGS       -DINSTALL_DOC=OFF -DBUILD_ROOTDICT=OFF -DBUILD_LCIO_TESTS=OFF -DBUILD_F77_TESTJOBS=OFF -DBUILD_LCIO_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=${LCIO_INSTALL_DIR}
        BUILD_COMMAND    ${CMAKE_MAKE_PROGRAM} -j4
        BUILD_IN_SOURCE  ON
    )
    set(LCIO_DIR ${LCIO_INSTALL_DIR} CACHE PATH "LCIO root dir" FORCE)
    set(LCIO_LIBRARIES ${LCIO_INSTALL_DIR}/lib/liblcio${CMAKE_SHARED_LIBRARY_SUFFIX} ${LCIO_INSTALL_DIR}/lib/libsio${CMAKE_SHARED_LIBRARY_SUFFIX} CACHE FILEPATH "LCIO libraries" FORCE)
    set(LCIO_INCLUDE_DIRS ${LCIO_INSTALL_DIR}/include CACHE PATH "LCIO include dirs" FORCE)
else()
    add_custom_target(LCIO) # dummy target
    message(STATUS "LCIO libraries found at: ${LCIO_LIBRARIES}")
endif()

add_dependencies(dependencies LCIO)
include_directories(${LCIO_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LCIO_LIBRARIES})
