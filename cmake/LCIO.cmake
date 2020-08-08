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
        SOURCE_DIR       "${CMAKE_BINARY_DIR}/lcio"
        CMAKE_ARGS       -DINSTALL_DOC=OFF -DBUILD_ROOTDICT=OFF -D BUILD_LCIO_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=${LCIO_INSTALL_DIR}
        BUILD_COMMAND    ${CMAKE_MAKE_PROGRAM} -j4
        BUILD_ALWAYS     ON
        BUILD_IN_SOURCE  ON
        EXCLUDE_FROM_ALL ON
    )
    set(LCIO_LIBRARIES ${LCIO_INSTALL_DIR}/lib/liblcio.so ${LCIO_INSTALL_DIR}/lib/libsio.so CACHE FILEPATH "LCIO libraries" FORCE)
    set(LCIO_INCLUDE_DIRS ${LCIO_INSTALL_DIR}/include CACHE PATH "LCIO include dirs" FORCE)
else()
    message(STATUS "LCIO libraries found at: ${LCIO_LIBRARIES}")
endif()

include_directories(${LCIO_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LCIO_LIBRARIES})