#!/bin/sh
#
# simple test driver that runs a list of executables in order
#

while [ ! "$#" -eq 0 ]; do
  if [ -x "$1" ]; then
   dir=`cd $(dirname $1) && pwd`
   file=$(basename $1)   
   $dir/$file && echo ""
  else
    echo "error: expected argument to be executable"
    exit 1
  fi

  shift
done
