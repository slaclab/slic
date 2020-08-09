mkdir build &> /dev/null
rm build/CMakeCache.txt &> /dev/null
cd build && cmake -DCMAKE_INSTALL_PREFIX=`realpath ../install` --log-level=DEBUG ..
make install
