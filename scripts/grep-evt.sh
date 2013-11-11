#!/bin/sh

if [ -n "$1" ]
then
  fname=$1
else
  fname="logfile"
fi

echo "filename: $fname"

if [ ! -e "$fname" ]; then
  echo "ERROR: $fname does not exist!"
  exit 1
fi

tail -f $@ | egrep "(BeginEvent|EndEvent)"
