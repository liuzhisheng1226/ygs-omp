#!/bin/bash

# usage: ./init_data.sh $1
# $1: dest directory

dest="$1"

echo "clean data in directory: /home/liutao/workspace/ygs_init_data/"
./clean_data.sh /home/liutao/workspace/ygs_init_data/
echo "clean done."

echo "clean data in directory: $dest"
rm -rf $dest
echo "clean done."

echo "copy data from /home/liutao/workspace/ygs_init_data/ to $dest" 
cp -r /home/liutao/workspace/ygs_init_data $dest
echo "copy done."

