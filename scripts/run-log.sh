#!/bin/sh
pre="."
if [ -n "$SLIC_BASE" ]; then
  pre=$SLIC_BASE
fi

run=$pre/bin/$G4SYSTEM/slic

if [ ! -e "$run" ]; then
  echo "$run does not exist!  Source your Geant4 setup script and/or run from \$SLIC_BASE."
else
  echo "exit" | $run $@ &> logfile
fi
