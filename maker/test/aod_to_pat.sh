#!/usr/bin/env bash
#
# Submit 1 condor job per file found in the input.txtf with given CMSSW config
#
# Created by Samvel Khalatyan, Aug 04, 2011
# Copyright 2011, All rights reserved

if [[ 3 -gt $# ]]
then
    echo Usage: `basename $0` input.txt cmssw_cfg.py PNFS/Folder [cmssw args]

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

proxy_file=`voms-proxy-info | grep path | awk '{print $3}'`
if [[ "x" == "x$proxy_file" ]]
then
    echo VOMS proxy is not setup

    exit 1
fi

proxy_hours_left=`voms-proxy-info | awk '/timeleft/{print $3}' | awk '{split($0, a, ":"); print a[1]}'`
if [[ 24 -gt $proxy_hours_left ]]
then
    while [[ 1 ]]
    do
        read -p "VOMS proxy is only valid in next ${proxy_hours_left} hours that is below 24h. Are you sure you want to continue (y/n)? " -n 1

        if [[ $REPLY =~ ^[Nn]$ ]]
        then
            echo stop submission: please re-initialize proxy

            exit 1
        elif [[ $REPLY =~ [Yy]$ ]]
        then
            break
        fi
    done
fi

prod_folder=prod_`date +%F_%R_%S | sed -e 's/[-:]/_/g'`
if [[ -d $prod_folder ]]
then
    echo Failed to start production. Output folder exists: $prod_folder

    exit 1
fi

pnfs_folder=$1/$prod_folder
shift

CMSRUN=`which cmsRun`
JOBS=`wc -l $input_file | cut -f1 -d' '`

mkdir $prod_folder
if [[ 0 -ne $? ]]
then
    echo Failed to create working folder: $prod_folder

    exit 1
fi

mkdir /pnfs/cms/WAX/11/store/user/$pnfs_folder
if [[ 0 -ne $? ]]
then
    echo Failed to create PNFS working folder: $pnfs_folder

    exit 1
fi

cp $input_file $prod_folder/input.txt
cp $config_file $prod_folder/cmssw_cfg.py
cp $proxy_file $prod_folder/proxy

eval "sed -e 's#CMSRUN#$CMSRUN#' ./run_aod_to_pat.sh &> $prod_folder/run.sh"
eval "sed -e 's#PWD#$PWD/$prod_folder#g' -e 's#ARGS#$pnfs_folder $@#' -e 's#JOBS#$JOBS#' ./condor.cfg &> $prod_folder/condor.cfg"

chmod u+x $prod_folder/run.sh

pushd $prod_folder &> /dev/null

for (( job=0; JOBS>job; job++ ))
do
    work_folder=job.$job
    mkdir $work_folder
    eval "sed -n '$(( job + 1 ))p' input.txt > $work_folder/input.txt"
    ln -s ../cmssw_cfg.py $work_folder/cmssw_cfg.py
    ln -s ../proxy $work_folder/proxy
done

condor_submit ./condor.cfg
popd &> /dev/null

exit 0
