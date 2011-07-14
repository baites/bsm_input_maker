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

CMSRUN=`which cmsRun`
JOBS=`wc -l $1 | cut -f1 -d' '`

mkdir $prod_folder

cp $1 $prod_folder/input.txt
cp $2 $prod_folder/cmssw_cfg.py

eval "sed -e 's#CMSRUN#$CMSRUN#' ./run.sh &> $prod_folder/run.sh"
eval "sed -e 's#PWD#$PWD/$prod_folder#g' -e 's#JOBS#$JOBS#' ./condor.cfg &> $prod_folder/condor.cfg"

chmod u+x $prod_folder/run.sh

pushd $prod_folder &> /dev/null

for (( job=0; JOBS>job; job++ ))
do
    work_folder=job.$job
    mkdir $work_folder
    eval "sed -n '$(( job + 1 ))p' input.txt > $work_folder/input.txt"
    cp cmssw_cfg.py $work_folder/cmssw_cfg.py
done

condor_submit ./condor.cfg
popd &> /dev/null

exit 0
