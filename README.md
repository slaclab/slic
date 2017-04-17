# SLIC

Simulation for the Linear Collider (SLIC) Geant4 simulation application

## Installation

For now the installation is done manually by first building SLIC's dependencies and then the main package.  Step-by-step instructions are given below.

### Tools

* gcc 4.8 or greater (prob 4.9 or greater is preferable)
    * You cannot use the default gcc on RHE6 or similarly old distros so you would need to install one yourself or use a dev toolset.
* CMake ~3.0 or greater (tested with 3.6)

### Initial Setup

There will be a `build` dir where all the packages are built and configured and an `install` dir where they are installed out of source.  
This structure can be setup as follows:

```
cd /scratch
mkdir slic; cd slic
mkdir install; cd install
export install_dir=$PWD
cd ..
mkdir build; cd build
```

### Quick Build Instructions

The SLIC build system is able to download and install all dependencies for the project if they are not found on your system.

Start by executing CMake from the build dir:

```
cd build
cmake ..
```

If no directory arguments were provided pointing CMake at locally installed packages, you will see a message stating "Some dependencies were not found."  This is not an error, but to complete the build you will need to build these dependencies and then rerun CMake so they are resolved.

To build the dependencies, execute the following:

```
make
```

Once this is done, then you need to rerun `cmake ..` from the build directory.  If the dependencies all installed successfully, then the message "All dependencies were found." should print.  Now you can just type `make; make install` to complete the build using these installed dependencies.

The default installation directory for SLIC and its dependencies is `~/slic` which you can change by providing an argument `-DGLOBAL_INSTALL_DIR=/my/install/dir` to the CMake command.

## Running SLIC

```
source /scratch/slic/install/slic/bin/slic-env.sh
slic [options]
```

## Installing Dependencies

Should you wish to install dependencies manually, the following instructions can be followed.

#### Geant4

Download the 10.3.p01 tarball from the Geant4 website and untar it or you may clone a tag from the Geant4 github.

```
cd geant4.10.03.p01; mkdir build; cd build
cmake -DGEANT4_INSTALL_DATA=ON -DCMAKE_INSTALL_PREFIX=$install_dir/geant4 ..
make -j4
make install
```

#### LCIO

```
git clone https://github.com/iLCSoft/LCIO.git lcio
cd lcio; mkdir build
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
./configure --prefix=/u/ey/jeremym/hps-dev/slic/install/xerces
make
make install
```

#### GDML

```
git clone https://github.com/slaclab/gdml
cd gdml; mkdir build; cd build
cmake -DGeant4_DIR=/u/ey/jeremym/hps-dev/slic/install/geant4/lib64/Geant4-10.3.1/ -DXERCES_DIR=/u/ey/jeremym/hps-dev/slic/install/xerces/ -DCMAKE_INSTALL_PREFIX=/u/ey/jeremym/hps-dev/slic/install/gdml ..
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
