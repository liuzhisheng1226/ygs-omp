#!/bin/bash

# usage: ./clean_data.sh path makefile
#           command        path   $mfname

path=""
mfname=""

if [ -z $1 ]
then
    echo "data root path:"
    read path
else
    path=$1
fi

if [ -z $path ] || [ ! -d $path ] 
then
    echo "need valid path!"
    exit
fi

if [ -z $2 ]
then
    echo "makefile name:"
    read mfname
else
    mfname=$2
fi

cd $path

if [ -z $mfname ]
then
    echo "need makefile name!"
    exit
elif [ $mfname = "all" ]
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
                rm -f integratePoint.rmg
                rm -f CTI/*.ldr CTI/*.rmg CTI/vel.txt CTI/velFileOut.txt CTI/deformationOut/*
                echo "all outdata in $path deleted!"
            ;;
            * )
                exit
            ;;
        esac
elif [ $mfname = "TestIQCombineMakefile" ]
then
    echo "delete TestIQCombineMakefile data"
    rm -f SLC/*.rmg SLC/*.ldr    
elif [ $mfname = "TestR2CalibrationMakefile" ]
then
    echo "delete TestR2CalibrationMakefile data"
    rm -f SLC/cal/*
elif [ $mfname = "TestInterferogramMapsMakefile" ]
then
    echo "delete TestInterferogramMapsMakefile data"
    rm -f inSARgraphy.txt
elif [ $mfname = "TestRegistrCoarseMakefile" ]
then
    echo "delete TestRegistrCoarseMakefile data"
    rm -f coaReg/*
elif [ $mfname = "TestRegistrFineMakefile" ]
then
    echo "delete TestRegistrFineMakefile data"
    rm -f fineReg/*.rmg fineReg/*.ldr
elif [ $mfname = "TestPreOrbImportMakefile" ]
then
    echo "delete TestPreOrbImportMakefile data"
    rm -f fineReg/*.orb
elif [ $mfname = "TestRoiSelectProMakefile" ]
then
    echo "delete TestRoiSelectProMakefile data"
    rm -f ROI/*
elif [ $mfname = "TestFlatPhaseMakefile" ]
then
    echo "delete TestFlatPhaseMakefile data"
    rm -f flatPha/*
elif [ $mfname = "TestCoherenceEstMakefile" ]
then
    echo "delete TestCoherenceEstMakefile data"
    rm -f coh/*.rmg coh/*.ldr
elif [ $mfname = "TestComplexProMakefile" ]
then
    echo "delete TestComplexProMakefile data"
    rm -f coh/amp/* coh/pha/*
elif [ $mfname = "TestMeanCohMakefile" ]
then
    echo "delete TestMeanCohMakefile data"
    rm -f CTI/meanCoh.*
elif [ $mfname = "TestMuskMakefile" ]
then
    echo "delete TestMuskMakefile data"
    rm -f CTI/CTmusk.*
elif [ $mfname = "TestBaselineMakefile" ]
then
    echo "delete TestBaselineMakefile data"
    rm -f CTI/BIR/*
elif [ $mfname = "TestPSCMakefile" ]
then
    echo "delete TestPSCMakefile data"
    rm -f CTI/PSC.*
elif [ $mfname = "TestDeluanayMakefile" ]
then
    echo "delete TestDeluanayMakefile data"
    rm -f CTI/edge.*
elif [ $mfname = "TestIntegrationMakefile" ]
then
    echo "delete TestIntegrationMakefile data"
    rm -f CTI/integratePoint.* CTI/vel.txt integratePoint.rmg
elif [ $mfname = "TestResidueMakefile" ]
then
    echo "delete TestResidueMakefile data(reset to TestIntegrationMakefile data)"
    #echo "TestResidueMakefile is special, run ./test.sh TestIntegrationMakefile to reset the data"
    rm -f CTI/integratePoint.rmg
    cp integratePoint.rmg CTI/integratePoint.rmg 
elif [ $mfname = "TestResidueUnwMakefile" ]
then
    echo "delete TestResidueUnwMakefile data"
    rm -f CTI/unwrapPoint.*
elif [ $mfname = "TestUonlinearMakefile" ]
then
    echo "delete TestUonlinearMakefile data"
    rm -f CTI/deformationOut/* CTI/velFileOut.txt
else
    echo "unknown option!"
fi
