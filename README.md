# SLIC

Simulation for the Linear Collider (SLIC) - Geant4 simulation application

## Installation

SLIC is a C++ application that is built using the standard GCC compiler toolchain and the CMake build generation system.

### Required Tools

* Recent gcc compiler toolchain (tested with GCC 7 on CentOS7)
    * You cannot use the default gcc on RHE6 or a similarly old distribution, so you would need to install one yourself or use a dev toolset.
    * The build is not compatible with gcc 4.8.5 now due to Geant4 updates.
* CMake 3.8 or greater due to requirements of the Geant4 build system (tested with 3.17)

### Build Instructions

Start by cloning the project to a local directory.

```
mkdir /scratch && cd /scratch
git clone https://github.com/slaclab/slic
```

Create the build directory within your slic project directory:

```
mkdir build && build
```

The SLIC build system is able to download and install all dependencies for the project if they are not found on your system. This requires a multi-step build for the dependencies to first be downloaded and installed before SLIC itself is built.

From the build dir created above execute the following:

```
cmake ..
```

You should see a number of messages printed that indicate dependencies were not found and will be built.

There are a few cmake options that can be used for configuring SLIC:

| Variable             | Default Value              | Description                           | Notes                   |
| -------------------- | ---------------------------| ------------------------------------- | ----------------------- |
| WITH_GEANT4_UIVIS    | NO                         | Enable Geant4 visualization and UI    | Requires X11 and Qt     |
| WITH_GEANT4_VERSION  | 10.6.1                     | Geant4 release tag to use             | `master` is not allowed |
| CMAKE_INSTALL_PREFIX | ${CMAKE_BUILD_DIR}/install | Installation prefix for slic and deps | |

Here is an example of all these options being used together:

```
cmake -DWITH_GEANT4_UIVIS=ON -DWITH_GEANT4_VERSION=10.6.1 -DCMAKE_INSTALL_PREFIX=/work
```

These options should be passed the first time you execute the `cmake` command when building the application.

Now to the dependencies and SLIC, execute the following:

```
make &> build.log
```

We save the build to a log file for debugging in case errors occur.

Now, you should be ready to install slic if the above completed without errors:

```
make install
```

To test your build, follow the instructions in the next section.

## Running SLIC

The build system generates a shell script that will setup the necessary environment for running the application:

```
source /scratch/slic/install/bin/slic-env.sh
```

Now you can run the executable from the command line:

```
slic [options]
```

Print the help menu:

```
slic --help
```

An actual command might look something like:

```
slic -g mygeom.lcdd -i events.stdhep -m commands.mac -r 10
```

Read the help to get an idea of the actual commands that are available.


### Specifying Dependencies

You may also have one or more of SLIC's dependencies installed locally, which you can use in your build by providing CMake with their root directories.

For instance, to use your own Geant4 installation, the command would be something like the following:

```
cmake -DGeant4_DIR=/path/to/geant4/lib64/Geant4-10.3.1/ ..
```

This table shows the full list of dependency variables accepted by SLIC:

| Dependency | Variable    |
| ---------- | ----------- |
| Geant4     | Geant4_DIR  |
| Xerces C++ | XercesC_DIR |
| LCIO       | LCIO_DIR    |
| HepPDT     | HEPPDT_DIR  |
| GDML       | GDML_DIR    |
| LCDD       | LCDD_DIR    |

Instructions for manually installing these dependencies are provided below.

## Installing Dependencies Manually

This section covers in detail the manual installation of SLIC's dependencies.  

These procedures are entirely optional, as running `cmake` without providing paths to pre-installed dependencies will cause them to be installed automatically as long as the CMake variable `INSTALL_DEPENDENCIES` is set to `ON`.

### Setup a Build Directory

Start by creating a directory to contain the build.

```
mkdir /scratch
cd /scratch
```

You will start from this directory when building each package by doing `cd /scratch`.

#### Geant4

```
git clone https://github.com/Geant4/geant4
cd geant4
git checkout v10.5.1 # choose Geant4 tag to build
mkdir build && cd build
# Geant4 cmake options may vary depending on your setup
cmake -DGEANT4_INSTALL_DATA=ON -DGEANT4_INSTALL_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=../install -DGEANT4_USE_SYSTEM_EXPAT=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
make -j8
make -j8 install
```

#### LCIO

```
git clone https://github.com/iLCSoft/LCIO.git lcio
cd lcio
git checkout v02-07-05
mkdir build && cd build
cmake -DINSTALL_DOC=OFF -DBUILD_LCIO_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=../install ..
make -j8 install
```

#### HepPDT

```
wget http://lcgapp.cern.ch/project/simu/HepPDT/download/HepPDT-3.04.01.tar.gz
tar -zxvf HepPDT-3.04.01.tar.gz
cd HepPDT-3.04.01
./configure --prefix=$PWD/install --disable-static
make -j8 install
```

#### Xerces

```
wget https://mirrors.ocf.berkeley.edu/apache//xerces/c/3/sources/xerces-c-3.2.3.tar.gz
tar -zxf xerces-c-3.2.3.tar.gz
cd xerces-c-3.2.3 && mkdir build && cd build
cmake -DBUILD_SHARED_LIBS=ON .. -DCMAKE_INSTALL_PREFIX=../install
make -j8 install
```

#### GDML

```
git clone https://github.com/slaclab/gdml
cd gdml && mkdir build && cd build
cmake -DGeant4_DIR=/scratch/geant4/install/lib64/Geant4-10.5.1/ -DXercesC_INCLUDE_DIR=/scratch/xerces/install/include -DXercesC_LIBRARY=/scratchu/xerces/install/lib64/libxerces-c.so -DCMAKE_INSTALL_PREFIX=../install ..
make -j8 install
```

### LCDD

```
git clone https://github.com/slaclab/lcdd
cd lcdd && mkdir build && cd build
cmake -DINSTALL_DOC=OFF -DGeant4_DIR=/scratch/geant4/install/lib64/Geant4-10.3.1/ -DGDML_DIR=/scratch/gdml/install -DXercesC_INCLUDE_DIR=/scratch/xerces/install/include -DXercesC_LIBRARY=/scratchu/xerces/install/lib64/libxerces-c.so -DCMAKE_INSTALL_PREFIX=../install ..
make -j4 install
```

### SLIC

```
git clone https://github.com/slaclab/slic
cd slic && mkdir build && cd build
cmake -DINSTALL_DOC=OFF -DCMAKE_INSTALL_PREFIX=../install -DXercesC_INCLUDE_DIR=/scratch/xerces/install/include -DXercesC_LIBRARY=/scratchu/xerces/install/lib64/libxerces-c.so -DLCIO_DIR=/scratch/lcio/install -DGeant4_DIR=/scratch/geant4/install/lib64/Geant4-10.3.1/ -DGDML_DIR=/scratch/gdml/install -DHEPPDT_DIR=/scratch/HepPDT-3.04.01/install -DLCDD_DIR=/scratch/lcdd/install ..
```
