#!/bin/bash

# usage: ./run_omp.sh num_threads
#            command     $1

thrds=1

if [ -z $1 ]
then
    echo "number of threads:"
    read thrds
else
    thrds=$1
fi

if [ $thrds -lt 1 ] || [ $thrds -gt 64 ]
then
    echo "invalid threads number!"
    exit
fi

#DATA_ROOT_PATH="/home/liutao/workspace/ygs_data/"
DATA_ROOT_PATH="/dev/shm/ygs_data/"
./clean_data.sh $DATA_ROOT_PATH all

TEST_PATH="/home/liutao/workspace/github/ygs-omp/UnitTest"
# run all tests.
cd $TEST_PATH
# date: YYYYmmddHHMMSS
date=$(date +%Y%m%d%H%M%S) 
logfile="/home/liutao/workspace/ygs_log/omp/log_omp_${thrds}_${date}"
touch $logfile

export OMP_NUM_THREADS=$thrds
export OMP_NESTED=true

for curfile in "TestIQCombineMakefile" "TestR2CalibrationMakefile" "TestInterferogramMapsMakefile" "TestRegistrCoarseMakefile" "TestRegistrFineMakefile" "TestPreOrbImportMakefile" "TestRoiSelectProMakefile" "TestFlatPhaseMakefile" "TestCoherenceEstMakefile" "TestComplexProMakefile" "TestMeanCohMakefile" "TestMuskMakefile" "TestBaselineMakefile" "TestPSCMakefile" "TestDeluanayMakefile" "TestIntegrationMakefile" "TestResidueMakefile" "TestResidueUnwMakefile" "TestUonlinearMakefile"
do
    sleep 3s 
    echo $curfile >> $logfile
    make -f $curfile 
    /usr/bin/time -p -a -o $logfile ./test
    make -f $curfile clean
    echo "" >> $logfile
done

touch "test"
make clean

echo "OMP_NUM_THREADS="$OMP_NUM_THREADS
