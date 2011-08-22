#!/usr/bin/env bash
#
# Submit 1 condor job per file found in the input.txtf with given CMSSW config
#
# Created by Samvel Khalatyan, Aug 04, 2011
# Copyright 2011, All rights reserved

if [[ 3 -gt $# ]]
then
    echo Usage: `basename $0` input.txt cmssw_cfg.py FILES_PER_JOB [cmssw args]

    exit 1
fi

if [[ !(-r condor.cfg) ]]
then
    echo condor.cfg file does not exist

    exit 1
fi

if [[ !(-r $1) ]]
then
    echo file $1 does not exist or not readable

    exit 1
fi

input_file=$1
shift

if [[ !(-r $1) ]]
then
    echo config $1 does not exist or not readable

    exit 1
fi

config_file=$1
shift

files_per_job=$1
shift

prod_folder=prod_`date +%F_%R_%S | sed -e 's/[-:]/_/g'`
if [[ -d $prod_folder ]]
then
    echo Failed to start production. Output folder exists: $prod_folder

    exit 1
fi

CMSRUN=`which cmsRun`
files=`wc -l $input_file | cut -f1 -d' '`
JOBS=$(( files / files_per_job ))
if [[ 0 -lt $(( files - JOBS * files_per_job )) ]]
then
    JOBS=$(( JOBS + 1 ))
fi

mkdir $prod_folder

cp $input_file $prod_folder/input.txt
cp $config_file $prod_folder/cmssw_cfg.py
touch $prod_folder/proxy

eval "sed -e 's#CMSRUN#$CMSRUN#' ./run_pat_to_pb.sh &> $prod_folder/run.sh"
eval "sed -e 's#PWD#$PWD/$prod_folder#g' -e 's#ARGS#$@#' -e 's#JOBS#$JOBS#' ./condor.cfg &> $prod_folder/condor.cfg"

chmod u+x $prod_folder/run.sh

pushd $prod_folder &> /dev/null

for (( job = 0; JOBS > job; ++job ))
do
    work_folder=job.$job
    mkdir $work_folder
    eval "awk 'NR > $(( job * files_per_job )) && NR <= $(( (job + 1) * files_per_job ))' input.txt > $work_folder/input.txt"
    ln -s ../cmssw_cfg.py $work_folder/cmssw_cfg.py
    ln -s ../proxy $work_folder/proxy
done

condor_submit ./condor.cfg
popd &> /dev/null

exit 0
