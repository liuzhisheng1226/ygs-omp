#!/bin/bash

# usage: ./init_data.sh src dest

echo "src path:"
read src

if [ -z $src ] || [ ! -d $src ]
then 
    echo "invalid src path!"
    exit
fi

echo "dest path:"
read dest

if [ -z $dest ] || [ ! -d $dest ]
then 
    echo "invalid dest path(dest should exist already)!"
    exit
fi

echo "clean data in directory: $src"
./clean_data.sh $src all
echo "clean done."

echo "clean data in directory: $dest"
rm -rf $dest
echo "clean done."

echo "copy data from /home/liutao/workspace/ygs_init_data/ to $dest" 
cp -r $src $dest
echo "copy done."

