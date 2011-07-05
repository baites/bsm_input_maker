#!/usr/bin/env bash
# Submit 1 condor job per file found in the input.txtf with given CMSSW config

if [[ 2 -ne $# ]]
then
    echo Usage: `basename $0` input.txt cmssw_cfg.py

    exit 1
fi

if [[ !(-r $1) ]]
then
    echo file $1 does not exist or not readable

    exit 1
fi

if [[ !(-r $2) ]]
then
    echo config $2 does not exist or not readable

    exit 1
fi

prod_folder=prod_`date +%F_%R_%S | sed -e 's/[-:]/_/g'`
if [[ -d $prod_folder ]]
then
    echo Failed to start production. Output folder exists: $prod_folder

    exit 1
fi

mkdir $prod_folder
file_number=0
CMSRUN=`which cmsRun`

while read line
do
    file_number=$(( file_number + 1 ))

    working_folder=$prod_folder/$file_number

    mkdir $working_folder

    echo $line > $working_folder/input.txt

    cp $2 $working_folder/cmssw_cfg.py

    eval "sed -e 's#PWD#$PWD/$working_folder#g' ./run.sh &> $working_folder/run.sh"
    eval "sed -e 's#PWD#$PWD/$working_folder#g' -e 's#CMSRUN#$CMSRUN#' ./condor.cfg &> $working_folder/condor.cfg"

    chmod u+x $working_folder/run.sh

    pushd $working_folder &> /dev/null

    condor_submit ./condor.cfg

    popd &> /dev/null
done < $1

exit 0
