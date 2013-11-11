#!/bin/sh

# $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/scripts/run-segtest.sh,v 1.1 2005-01-10 18:28:22 jeremy Exp $

echo "Running SLIC segmentation tests..."
echo

for s in NPCylBarr.mac NPPolyBarr.mac NPPolyEcap.mac PrjBarr.mac PrjEcap.mac; do
  echo $s
  logname=`echo $s | sed s/.mac//g`
  logname=${logname}_log
  ./scripts/run-log.sh macros/$s
  mv logfile $logname
  echo "logfile: $logname"
  echo
done

echo "Done."
