#!/bin/sh

# set G4SYSTEM by guessing (should be right)
if [ -z "$G4SYSTEM" ]; then
  g4sys=`uname`-g++
# or from env if set
else
  g4sys=$G4SYSTEM
fi

# check for SLIC_BASE
if [ -z "$SLIC_BASE" ]; then
  echo "ERROR: Set SLIC_BASE to slic's root area."
  exit 1 
fi 

# executable path
cmd=$SLIC_BASE/bin/$g4sys/slic

# check for executable
if [ ! -e "$cmd" ]; then
  echo "slic executable not found at $cmd.  Did you build slic?"  
  exit 1
fi

# check if can exec
if [ ! -x "$cmd" ]; then
  echo "slic binary not executable.  Permissions problem?"
  exit 1
fi

# set LD path because Xerces is shared lib
xml="$XERCESCROOT"
if [ -n "$xml" ]; then
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$XERCESCROOT/lib
  echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
fi

# If XERCESCROOT was not found in env, SLIC might fail to load unless 
# $XERCESROOT/lib is in LD_LIBRARY_PATH.

# exec with all CL
time $cmd $@
