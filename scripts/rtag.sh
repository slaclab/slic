#!/bin/sh

if [ -z "${SLIC_BASE}" ]; then
  echo "ERROR: Set SLIC_BASE."
else

  if [ -e ${SLIC_BASE}/VERSION ]; then
    appname=`cat ${SLIC_BASE}/VERSION | awk '{print $1}'`
    ver=`cat ${SLIC_BASE}/VERSION | awk '{print $2}'`

    echo "appname=$appname"
    echo "ver=$ver"

    ver_okay=`echo $ver | egrep "^v[0-9]*r[0-9]*p[0-9_]*$"`

    if [ -n "${ver_okay}" ]; then
      cmd="cvs rtag $ver_okay slic"
      echo "exec: $cmd"
      echo "Press ENTER to rtag..."
      read
      $cmd
    else
       echo "$ver does not match v#r#p#[_#] format."
    fi

  else
    echo "ERROR: Could not find VERSION."
  fi

fi
