#!/bin/bash

# usage: ./run_omp.sh num_threads
#            command     $1

DATA_ROOT_PATH="/home/liutao/workspace/ygs_data/"
TEST_PATH="/home/liutao/workspace/github/ygs-omp/UnitTest"

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

# run all tests.
cd ${TEST_PATH}
# date: YYYYmmddHHMMSS
date=$(date +%Y%m%d%H%M%S) 
logfile="log/log_omp_${1}_${date}"
touch $logfile

export OMP_NUM_THREADS=$1

for curfile in "TestIQCombineMakefile" "TestR2CalibrationMakefile" "TestInterferogramMapsMakefile" "TestRegistrCoarseMakefile" "TestRegistrFineMakefile" "TestPreOrbImportMakefile" "TestRoiSelectProMakefile" "TestFlatPhaseMakefile" "TestCoherenceEstMakefile" "TestComplexProMakefile" "TestMeanCohMakefile" "TestMuskMakefile" "TestBaselineMakefile" "TestPSCMakefile" "TestDeluanayMakefile" "TestIntegrationMakefile" "TestResidueMakefile" "TestResidueUnwMakefile" "TestUonlinearMakefile"
do
    echo $curfile >> $logfile
    make -f $curfile 
    /usr/bin/time -p -a -o $logfile ./test
    make -f $curfile clean
    echo "" >> $logfile
    sleep 10s 
done

echo "OMP_NUM_THREADS="${OMP_NUM_THREADS}
