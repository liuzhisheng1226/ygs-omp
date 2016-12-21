#!/bin/bash

# usage: ./clean_data.sh

DATA_ROOT_PATH="/home/liutao/workspace/ygs_data/"

# clean data
cd ${DATA_ROOT_PATH}
rm -rf SLC
mkdir -p SLC/cal
rm -rf coaReg
mkdir coaReg
rm -rf fineReg
mkdir fineReg
rm -rf coh
mkdir -p coh/amp coh/pha
rm -rf flatPha
mkdir flatPha
rm -f inSARgraphy.txt
rm -rf ROI
mkdir ROI
rm -rf CTI/BIR
mkdir CTI/BIR
rm -f CTI/*.ldr CTI/*.rmg CTI/vel.txt CTI/velFileOut.txt CTI/deformationOut/*

