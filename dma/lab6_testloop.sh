#/* **************** LDD:2.0 chap15/lab6_testloop.sh **************** */
#/*
# *
# */
#!/bin/bash

# 2/2008 J. Cooperstein (coop@linuxfoundation.org) License:GPLv2

file="/dev/mycdrv" && [[ -n $1 ]] && file=$1
reps=10            && [[ -n $2 ]] && reps=$2

echo DOING $reps iterations on $file with ../lab1_aio_test:
time  ( n=0 ; \
while [[ $n -lt $reps ]]
  do ./lab1_aio_test $file > /dev/null
  n=$(($n+1))
  sync
done )

echo DOING $reps iterations on $file with ../lab1_posix_test:
time  ( n=0; \
while [[ $n -lt $reps ]]
  do ./lab1_posix_test $file > /dev/null
  n=$(($n+1))
  sync
done )
