message(STATUS "Checking for LCIO")
find_package(LCIO QUIET)
if(NOT LCIO_FOUND)
    message(STATUS "LCIO was not found and will be installed")
    add_dependencies(dependencies LCIO)
    set(LCIO_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/LCIO)
    externalproject_add(
        LCIO
        GIT_REPOSITORY   "https://github.com/iLCSoft/LCIO"
        GIT_TAG          "v02-07-05"
        UPDATE_COMMAND   ""
        PATCH_COMMAND    ""
        SOURCE_DIR       ${CMAKE_BINARY_DIR}/LCIO
        CMAKE_ARGS       -DINSTALL_DOC=OFF -DBUILD_ROOTDICT=OFF -DBUILD_LCIO_TESTS=OFF -DBUILD_F77_TESTJOBS=OFF -DBUILD_LCIO_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=${LCIO_INSTALL_DIR}
        BUILD_COMMAND    ${CMAKE_MAKE_PROGRAM} -j4
        BUILD_ALWAYS     ON
        EXCLUDE_FROM_ALL ON
    )
    set(LCIO_DIR ${LCIO_INSTALL_DIR} CACHE PATH "LCIO root dir" FORCE)
    set(LCIO_LIBRARIES ${LCIO_INSTALL_DIR}/lib/liblcio.so ${LCIO_INSTALL_DIR}/lib/libsio.so CACHE FILEPATH "LCIO libraries" FORCE)
    set(LCIO_INCLUDE_DIRS ${LCIO_INSTALL_DIR}/include CACHE PATH "LCIO include dirs" FORCE)
else()
    message(STATUS "LCIO libraries found at: ${LCIO_LIBRARIES}")
endif()

include_directories(${LCIO_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LCIO_LIBRARIES})
