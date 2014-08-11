valgrind -v \
--demangle=yes \
--gen-suppressions=all \
--leak-check=yes \
--num-callers=40 \
--leak-resolution=high \
--show-reachable=yes \
--tool=memcheck \
--log-file-exactly=valgrind.log \
--suppressions=./valgrind.supp \
./build/bin/slic $@

# gdb
#--gdb-attach=yes \
