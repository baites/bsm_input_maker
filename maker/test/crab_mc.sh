#! /usr/bin/env bash

if [[ 2 -gt $# ]]
then
    echo Usage: `basename $0` cmssw_cfg.py DATASET [cmssw args]

    exit 1
fi

pset=$1
if [[ !(-r "${pset}") ]]
then
    echo "CMSSW config '${pset}' file does not exist"

    exit 1
fi

dataset=$2

prod_folder=`date +%F_%R_%S | sed -e 's/[-:]/_/g'`

eval "sed -e 's#DATASET#${dataset}#g' \
    -e 's#CONFIG#${pset}#' \
    -e 's#FOLDER#${prod_folder}#' \
    -e 's#ARGS#${@:3}#' ./crab_mc.cfg &> crab.cfg"

exit 0
