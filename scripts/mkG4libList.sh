#!/bin/sh

source config/setup/setup-slic-slac.sh

./configure --enable-compile-verbose

gmake all &> gmake.log

grep succeeded gmake.log | sed -e 's/attempt to open //' -e 's/succeeded//' &> link_libs

> g4_link_libs
for l in `cat link_libs`; do
  basename $l | grep -v G4Processor | grep -v G4Subscribers | grep G4 | sed -e 's/lib//' -e 's/\.a$//' -e 's/\.so$//' >> g4_link_libs
done

# don't sort -- maintain order for static linking
cat g4_link_libs | uniq > g4_link_libs.tmp
mv g4_link_libs.tmp g4_link_libs

#egrep -v "[G4Processor|G4Subscriber]"
