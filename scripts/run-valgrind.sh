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
$SLIC_BASE/bin/$G4SYSTEM/slic $@

# gdb
#--gdb-attach=yes \
