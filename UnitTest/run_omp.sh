#!/bin/bash

# usage: ./run_omp.sh num_threads
#            command     $1

#DATA_ROOT_PATH="/home/liutao/workspace/ygs_data/"
./clean_data.sh


TEST_PATH="/home/liutao/workspace/github/ygs-omp/UnitTest"
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

touch "test"
make clean

echo "OMP_NUM_THREADS="${OMP_NUM_THREADS}
