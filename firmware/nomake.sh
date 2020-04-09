#/* **************** LDD:2.0 chap14/nomake.sh **************** */
#/*
# *
# */
#!/bin/bash

PATH=../:$PATH
cat *.c > /lib/firmware/my_fwfile
genmake $KROOT
make
