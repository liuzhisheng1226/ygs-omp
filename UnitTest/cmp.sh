#!/bin/bash

# usage: ./cmp.sh mfname
#           cmd    $1

LINUX_DATA_PATH="/dev/shm/ygs_linux_data/"
OMP_DATA_PATH="/dev/shm/ygs_data/"

mfname=""

if [ -z $1 ]
then
    echo "input makefile name as info for compare:"
    read mfname 
else
    mfname=$1
fi

cmpfiles()
{
    restpath=$1
    restdir=`dirname $restpath[0]`
    filelist=$LINUX_DATA_PATH$restpath
    findingdir=$OMP_DATA_PATH$restdir
    #echo $filelist
    #echo $findingdir
    for file1 in $filelist
    do
        file2="${findingdir}/$(basename $file1)"
        #echo "file2: $file2"
        if [ -e $file2 ]
        then 
            echo "compare $file1 with $file2 ..."
            cmp_res=$(cmp -s $file1 $file2 && echo "same" || echo "diff")
            echo $cmp_res
        else
            echo "$file2 doesn't exist!"
            exit
        fi
    done
}

#cmpfiles SLC/*.ldr 

if [ -z $mfname ]
then
    echo "need makefile name!"
    exit
elif [ $mfname = "TestIQCombineMakefile" ]
then
    echo "compare TestIQCombineMakefile data"
    cmpfiles SLC/*.rmg
    cmpfiles SLC/*.ldr
elif [ $mfname = "TestR2CalibrationMakefile" ]
then
    echo "compare TestR2CalibrationMakefile data"
    cmpfiles SLC/cal/*
elif [ $mfname = "TestInterferogramMapsMakefile" ]
then
    echo "compare TestInterferogramMapsMakefile data"
    cmpfiles inSARgraphy.txt
elif [ $mfname = "TestRegistrCoarseMakefile" ]
then
    echo "compare TestRegistrCoarseMakefile data"
    cmpfiles coaReg/*
elif [ $mfname = "TestRegistrFineMakefile" ]
then
    echo "compare TestRegistrFineMakefile data"
    cmpfiles fineReg/*.rmg
    cmpfiles fineReg/*.ldr
elif [ $mfname = "TestPreOrbImportMakefile" ]
then
    echo "compare TestPreOrbImportMakefile data"
    cmpfiles fineReg/*.orb
elif [ $mfname = "TestRoiSelectProMakefile" ]
then
    echo "compare TestRoiSelectProMakefile data"
    cmpfiles ROI/*
elif [ $mfname = "TestFlatPhaseMakefile" ]
then
    echo "compare TestFlatPhaseMakefile data"
    cmpfiles flatPha/*
elif [ $mfname = "TestCoherenceEstMakefile" ]
then
    echo "compare TestCoherenceEstMakefile data"
    cmpfiles coh/*.rmg
    cmpfiles coh/*.ldr
elif [ $mfname = "TestComplexProMakefile" ]
then
    echo "compare TestComplexProMakefile data"
    cmpfiles coh/amp/*
    cmpfiles coh/pha/*
elif [ $mfname = "TestMeanCohMakefile" ]
then
    echo "compare TestMeanCohMakefile data"
    cmpfiles CTI/meanCoh.*
elif [ $mfname = "TestMuskMakefile" ]
then
    echo "compare TestMuskMakefile data"
    cmpfiles CTI/CTmusk.*
elif [ $mfname = "TestBaselineMakefile" ]
then
    echo "compare TestBaselineMakefile data"
    cmpfiles CTI/BIR/*
elif [ $mfname = "TestPSCMakefile" ]
then
    echo "compare TestPSCMakefile data"
    cmpfiles CTI/PSC.*
elif [ $mfname = "TestDeluanayMakefile" ]
then
    echo "compare TestDeluanayMakefile data"
    cmpfiles CTI/edge.*
elif [ $mfname = "TestIntegrationMakefile" ]
then
    echo "compare TestIntegrationMakefile data"
    cmpfiles CTI/intregatePoint.* 
    cmpfiles CTI/vel.txt
elif [ $mfname = "TestResidueMakefile" ]
then
    echo "compare TestResidueMakefile data"
    cmpfiles CTI/intregatePoint.* 
elif [ $mfname = "TestResidueUnwMakefile" ]
then
    echo "compare TestResidueUnwMakefile data"
    cmpfiles CTI/unwrapPoint.*
elif [ $mfname = "TestUonlinearMakefile" ]
then
    echo "compare TestUonlinearMakefile data"
    cmpfiles CTI/deformationOut/* 
    cmpfiles CTI/velFileOut.txt
else
    echo "unknown option!"
fi






