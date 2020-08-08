message(STATUS "Checking for XercesC")
find_package(XercesC QUIET)
if(NOT XercesC_FOUND)
    message(STATUS "XercesC was not found and will be installed")
    add_dependencies(dependencies XercesC)
    set(XercesC_VERSION 3.2.3 CACHE STRING "XercesC version" FORCE)
    set(XercesC_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/XercesC)
    externalproject_add(
        XercesC
        URL                  "https://downloads.apache.org/xerces/c/3/sources/xerces-c-${XercesC_VERSION}.tar.gz"
        SOURCE_DIR           ${CMAKE_BINARY_DIR}/XercesC
        CONFIGURE_COMMAND    ./configure --enable-shared --disable-static --prefix=${XercesC_INSTALL_DIR}
        BUILD_IN_SOURCE      ON
        BUILD_ALWAYS         ON
        EXCLUDE_FROM_ALL     ON
        DOWNLOAD_NO_PROGRESS ON
    )
    set(XercesC_LIBRARY ${XercesC_INSTALL_DIR}/lib/libxerces-c.so CACHE FILEPATH "XercesC libraries" FORCE)
    set(XercesC_INCLUDE_DIR ${XercesC_INSTALL_DIR}/include CACHE PATH "XercesC include dir" FORCE)
else()
    message(STATUS "XercesC library was found at: ${XercesC_LIBRARY}")
endif()

include_directories(${XercesC_INCLUDE_DIR})
set(LIBS ${LIBS} ${XercesC_LIBRARY})
