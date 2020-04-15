# SLIC

Simulation for the Linear Collider (SLIC) - Geant4 simulation application

## Installation

SLIC is a C++ application that is built using the standard GCC compiler toolchain and the CMake build generation system.

### Required Tools

* Recent gcc compiler toolchain (tested with GCC 7 on CentOS7)
    * You cannot use the default gcc on RHE6 or similarly old distros, so you would need to install one yourself or use a dev toolset.
    * The build is not compatible with gcc 4.8.5 now due to Geant4 updates.
* CMake 3.8 or greater due to Geant4 build system (tested with 3.17)

### Initial Setup

You will create a build directory where all the required packages are configured and compiled.

Create the build directory within your slic project directory:

```
mkdir build
```

All build commands will be executed from within this build directory (*not* from the slic project directory).

### Basic Build Instructions

The SLIC build system is able to download and install all dependencies for the project if they are not found on your system.

Start by executing CMake from the build dir:

```
cd slic/build
cmake -DCMAKE_INSTALL_PREFIX=/my/slic/install/dir -DGEANT4_INSTALL_DATA=ON -DINSTALL_DEPENDENCIES=ON ..
```

If no directory arguments are provided that point to locally installed packages, you will see a message stating "Some dependencies were not found."  This is not an error, but to complete the build you will need to build these dependencies and then rerun CMake so they are resolved by the build system.

Now to build the dependencies, execute the following:

```
make
```

Once this is done, then you need to rerun `cmake ..` from the build directory.  If the dependencies all installed successfully, then the message "All dependencies were found." should print.  Now you can just type `make; make install` to complete the build using these installed dependencies.

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
| Xerces C++ | XERCES_DIR  |
| LCIO       | LCIO_DIR    |
| HepPDT     | HEPPDT_DIR  |
| GDML       | GDML_DIR    |
| LCDD       | LCDD_DIR    |

Instructions for manually installing these dependencies are given below.

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
git checkout v10.6.1
mkdir build && cd build
cmake -DGEANT4_INSTALL_DATA=ON -DCMAKE_INSTALL_PREFIX=../install ../geant4
make -j4
make install
```

#### LCIO

```
git clone https://github.com/iLCSoft/LCIO.git lcio
cd lcio; git checkout v02-06
mkdir build; cd build
cmake -DINSTALL_DOC=OFF -DCMAKE_INSTALL_PREFIX=$install_dir/lcio ..
make -j4
make install
```

#### HepPDT

```
wget http://lcgapp.cern.ch/project/simu/HepPDT/download/HepPDT-3.04.01.tar.gz
tar -zxvf HepPDT-3.04.01.tar.gz
cd HepPDT-3.04.01
./configure --prefix=/u/ey/jeremym/hps-dev/slic/install/heppdt .. --disable-static
make install
```

#### Xerces

```
wget https://mirrors.ocf.berkeley.edu/apache//xerces/c/3/sources/xerces-c-3.2.3.tar.gz
./configure --prefix=/u/ey/jeremym/hps-dev/slic/install/xerces
make
make install
```

#### GDML

```
git clone https://github.com/slaclab/gdml
mkdir build && cd build
cmake -DGeant4_DIR=/u/ey/jeremym/hps-dev/slic/install/geant4/lib64/Geant4-10.3.1/ -DXercesC_INCLUDE_DIR=/u/ey/jeremym/hps-dev/slic/install/xerces/include -DXercesC_LIBRARY=/u/ey/jeremym/hps-dev/slic/install/xerces/lib64/libxerces-c.so -DCMAKE_INSTALL_PREFIX=/u/ey/jeremym/hps-dev/slic/install/gdml ..
make -j4 install
```

### LCDD

```
git clone https://github.com/slaclab/lcdd
cd lcdd; mkdir build; cd build
cmake -DINSTALL_DOC=OFF -DGeant4_DIR=$install_dir/geant4/lib64/Geant4-10.3.1/ -DGDML_DIR=$install_dir/gdml -DXERCES_DIR=$install_dir/xerces -DCMAKE_INSTALL_PREFIX=$install_dir/gdml ..
make -j4 install
```

### SLIC

```
cmake -DINSTALL_DOC=OFF -DCMAKE_INSTALL_PREFIX=$install_dir/slic -DXERCES_DIR=$install_dir/xerces -DLCIO_DIR=$install_dir/lcio/ -DGeant4_DIR=$install_dir/geant4/lib64/Geant4-10.3.1/ -DGDML_DIR=$install_dir/gdml/ -DHEPPDT_DIR=$install_dir/heppdt -DLCDD_DIR=$install_dir/lcdd ..
```
