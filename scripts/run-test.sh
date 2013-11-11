#!/bin/sh
echo "exit" | $G4WORKDIR/bin/$G4SYSTEM/slic $G4WORKDIR/macros/test.mac &> logfile
