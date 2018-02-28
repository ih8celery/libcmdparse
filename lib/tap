#!/bin/sh
#
# simple test driver that runs a list of executables in order
#

while [ ! "$#" -eq 0 ]; do
  if [ -d "$1" ]; then
    for fname in "$1"/*; do
      [ -f "$fname" -a -x "$fname" ] && $fname
    done
  elif [ -f "$1" -a -x "$1" ]; then
   dir=`cd $(dirname $1) && pwd`
   file=$(basename $1)   
   $dir/$file
  else
    echo "error: expected argument to be executable"
    exit 1
  fi

  shift
done
