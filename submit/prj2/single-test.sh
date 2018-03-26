#!/bin/bash

#Does a single hide/unhide test.  Specifically, given arguments
#N_PROCESSES, MSG_PATH and one-or-more IMG_PATH, hides message
#given by MSG_PATH using images IMG_PATH... with resulting images put in
#a empty dir _out.  Then unhides message from _out dir images into
#a msg.txt file in _out dir.  Finally it compares the unhidden message
#with the original.  

OUT_DIR=./_out
UNHIDE_MSG_PATH=$OUT_DIR/msg.txt

PROG=./multi-steg

if [ $# -lt 3  ] || [ $1 == '-v' -a $# -lt 4 ]
then
  echo "usage: $0 [-v] N_PROCESSES MSG_PATH IMG_PATH..."
  exit 1;
fi

[ $1 == '-v' ] && VERBOSE=true || VERBOSE=false
$VERBOSE && shift
  
N_PROCESSES=$1; shift
MSG_PATH=$1; shift
IMG_PATHS="$@"

COMMANDS=(
  "rm -rf $OUT_DIR"
  "mkdir $OUT_DIR"
  "rm -f $OUT_DIR/*"
  "$PROG hide $N_PROCESSES $IMG_PATHS $MSG_PATH $OUT_DIR"
  "$PROG unhide $N_PROCESSES $OUT_DIR/*.ppm $UNHIDE_MSG_PATH"
  "cmp $MSG_PATH $UNHIDE_MSG_PATH"
)

for cmd in "${COMMANDS[@]}"
do
  $VERBOSE && echo $cmd  
  `$cmd`
  if [ $? -ne 0 ]
  then
    echo "$cmd failed"
    echo "failed $0 $N_PROCESSES $MSG_PATH $IMG_PATHS"
    exit 1
  fi
done

