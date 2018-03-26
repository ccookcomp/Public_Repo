#!/bin/sh

COURSE=cs551
AUX_DIR=$HOME/$COURSE/projects/prj2/aux
IMG_DIR=$AUX_DIR/images
MSG_DIR=$AUX_DIR/messages

PROG=./single-test.sh

ONE_IMG=$IMG_DIR/rose.ppm
ALL_IMG=$IMG_DIR/*.ppm

N_PROCESSES="1 2 3 4 5 7 11 13 15 16"
MSGS=$MSG_DIR/*.txt

case "z$1" in
  z-v)
      VERBOSE=-v
      ;;
  z-h)
      echo "usage: $0 [-v|-h]"
      echo "runs single-test for cross-product of"
      echo "  N_PROCESSES in $N_PROCESSES"
      echo "  IMG in $ONE_IMG $ALL_IMG"
      echo "  MSG in `echo $MSGS`"
      exit 0
      ;;
  z)
      ;;
  *)
      echo "invalid option $1"
      exit 1
esac

#[ "$1" = '-v' ] && VERBOSE=-v

for n in $N_PROCESSES
do
  for img in $ONE_IMG "$ALL_IMG"
  do
    for msg in $MSGS
    do
      $PROG $VERBOSE $n $msg $img
    done
  done
done

