#!/bin/bash
# A Bash script for installing scullbuffer device

if [ $# -lt 1 ]
then
  echo Must provide a max buffer size
  exit
fi


if [ $1 -lt 1 ]
then
  echo Buffer size must be at least 1
  exit
fi

make
make producer
make consumer
chmod +x unload.py load.py
sudo ./unload.py
sudo ./load.py scull_size=$1
