#!/bin/bash

# usage: ./clean_data.sh path makefile
#           command        $1   $2

DATA_ROOT_PATH="$1"

cd ${DATA_ROOT_PATH}

if [ -z $2 ]
then
    read -p "do you want to deleta all data?(y/n) " yn
        case $yn in
            y|Y )
                # clean all data
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
            ;;
            * )
                exit
            ;;
        esac
elif [ $2 = "TestIQCombineMakefile" ]
then
    echo "delete TestIQCombineMakefile data"
    rm -f SLC/*.rmg SLC/*.ldr    
elif [ $2 = "TestR2CalibrationMakefile" ]
then
    echo "delete TestR2CalibrationMakefile data"
    rm -f SLC/cal/*
elif [ $2 = "TestInterferogramMapsMakefile" ]
then
    echo "delete TestInterferogramMapsMakefile data"
    rm -f inSARgraphy.txt
elif [ $2 = "TestRegistrCoarseMakefile" ]
then
    echo "delete TestRegistrCoarseMakefile data"
    rm -f coaReg/*
elif [ $2 = "TestRegistrFineMakefile" ]
then
    echo "delete TestRegistrFineMakefile data"
    rm -f fineReg/*.rmg fineReg/*.ldr
elif [ $2 = "TestPreOrbImportMakefile" ]
then
    echo "delete TestPreOrbImportMakefile data"
    rm -f fineReg/*.orb
elif [ $2 = "TestRoiSelectProMakefile" ]
then
    echo "delete TestRoiSelectProMakefile data"
    rm -f ROI/*
elif [ $2 = "TestFlatPhaseMakefile" ]
then
    echo "delete TestFlatPhaseMakefile data"
    rm -f flatPha/*
elif [ $2 = "TestCoherenceEstMakefile" ]
then
    echo "delete TestCoherenceEstMakefile data"
    rm -f coh/*.rmg coh/*.ldr
elif [ $2 = "TestComplexProMakefile" ]
then
    echo "delete TestComplexProMakefile data"
    rm -f coh/amp/* coh/pha/*
elif [ $2 = "TestMeanCohMakefile" ]
then
    echo "delete TestMeanCohMakefile data"
    rm -f CTI/meanCoh.*
elif [ $2 = "TestMuskMakefile" ]
then
    echo "delete TestMuskMakefile data"
    rm -f CTI/CTmusk.*
elif [ $2 = "TestBaselineMakefile" ]
then
    echo "delete TestBaselineMakefile data"
    rm -f CTI/BIR/*
elif [ $2 = "TestPSCMakefile" ]
then
    echo "delete TestPSCMakefile data"
    rm -f CTI/PSC.*
elif [ $2 = "TestDeluanayMakefile" ]
then
    echo "delete TestDeluanayMakefile data"
    rm -f CTI/edge.*
elif [ $2 = "TestIntegrationMakefile" ]
then
    echo "delete TestIntegrationMakefile data"
    rm -f CTI/intregatePoint.* CTI/vel.txt
elif [ $2 = "TestResidueMakefile" ]
then
    echo "delete TestResidueMakefile data"
    echo "TestResidueMakefile is special, run ./test.sh TestIntegrationMakefile to reset the data"
elif [ $2 = "TestResidueUnwMakefile" ]
then
    echo "delete TestResidueUnwMakefile data"
    rm -f CTI/unwrapPoint.*
elif [ $2 = "TestUonlinearMakefile" ]
then
    echo "delete TestUonlinearMakefile data"
    rm -f CTI/deformationOut/* CTI/velFileOut.txt
else
    echo "unknown option!"
fi
