#/* **************** LDD:2.0 chap15/nomake.sh **************** */
#/*
# *
# */
#!/bin/bash

PATH=../:$PATH
export LDLIBS="-lrt -laio"
genmake $KROOT
make
