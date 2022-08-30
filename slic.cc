/*!
  * @mainpage Simulator for the Linear Collider (SLIC)
  *
  * @par
  * The SLIC simulator is a Geant4 package that uses LCDD as its geometry input.
  *
  * @par
  * To see the available command line options, execute the following from the shell:
  * @verbatim slic --help @endverbatim
  *
  * @par
  * To print the version of the application, execute the following:
  * @verbatim slic --version @endverbatim
  *
  * @par
  * The primary source of documentation for SLIC is its
  * <a href="https://confluence.slac.stanford.edu/display/ilc/SLIC">Confluence Wiki page</a>.
  *
  * @par
  * Any questions can be directed to <a href="mailto:jeremym@slac.stanford.edu">Jeremy McCormick</a> or <a href="mailto:norman.graf@slac.stanford.edu">Norman Graf</a>.
  * 
  * @section installation Installation
  * 
  * SLIC is a C++ application that is built using the standard GCC compiler toolchain and the CMake build generation system.
  * 
  * @subsection req-tools Required Tools
  * 
  * - Recent gcc compiler toolchain (tested with GCC 7 on CentOS7)
  *   - You cannot use the default gcc on RHE6 or a similarly old distribution, so you would need to install one yourself or use a dev toolset.
  *   - The build is not compatible with gcc 4.8.5 now due to Geant4 updates.
  * - CMake 3.8 or greater due to requirements of the Geant4 build system (tested with 3.17)
  * 
  * @subsection build Build Instructions
  * 
  * Start by cloning the project to a local directory.
  * ```
  * mkdir /scratch && cd /scratch
  * git clone https://github.com/slaclab/slic
  * ```
  *
  * Create the build directory within your slic project directory:
  * ```
  * mkdir build && build
  * ```
  *
  * The SLIC build system is able to download and install all dependencies for the project if they are not found on your system.
  * This requires a multi-step build for the dependencies to first be downloaded and installed before SLIC itself is built.
  *
  * From the build dir created above execute the following:
  * ```
  * cmake ..
  * ```
  * You should see a number of messages printed that indicate dependencies were not found and will be built.
  *
  * There are a few cmake options that can be used for configuring SLIC:
  *
  * | Variable             | Default Value              | Description                           | Notes                   |
  * | -------------------- | ---------------------------| ------------------------------------- | ----------------------- |
  * | WITH_GEANT4_UIVIS    | NO                         | Enable Geant4 visualization and UI    | Requires X11 and Qt     |
  * | WITH_GEANT4_VERSION  | 10.6.1                     | Geant4 release tag to use             | `master` is not allowed |
  * | CMAKE_INSTALL_PREFIX | ${CMAKE_BUILD_DIR}/install | Installation prefix for slic and deps | |
  *
  * Here is an example of all these options being used together:
  * ```
  * cmake -DWITH_GEANT4_UIVIS=ON -DWITH_GEANT4_VERSION=10.6.1 -DCMAKE_INSTALL_PREFIX=/work
  * ```
  * These options should be passed the first time you execute the `cmake` command when building the application.
  *
  * Now to the dependencies and SLIC, execute the following:
  * ```
  * make
  * ```
  * Now, you should be ready to install slic if the above completed without errors:
  * ```
  * make install
  * ```
  * 
  * For further information see <a href="https://github.com/slaclab/slic/blob/master/README.md">SLIC README</a> on github.
  */

// SLIC
#include "SlicMain.hh"

using namespace slic;

/**
 * The main function for the application, which delegates to a class.
 */
int main(int argc, char** argv) {
    return (new SlicMain())->main(argc, argv);
}


