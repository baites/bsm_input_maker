#! /usr/bin/env bash

if [[ 3 -gt $# ]]
then
    echo Usage: `basename $0` cmssw_cfg.py DATASET LumiJSON [cmssw args]

    exit 1
fi

pset=$1
if [[ !(-r "${pset}") ]]
then
    echo "CMSSW config '${pset}' file does not exist"

    exit 1
fi

dataset=$2
lumi=$3
if [[ !(-r "${lumi}") ]]
then
    echo "Lumi sections '${lumi}' JSON file does not exist"

    exit 1
fi

prod_folder=`date +%F_%R_%S | sed -e 's/[-:]/_/g'`

eval "sed -e 's#DATASET#${dataset}#g' \
    -e 's#CONFIG#${pset}#' \
    -e 's#LUMI_MASK#${lumi}#' \
    -e 's#FOLDER#${prod_folder}#' \
    -e 's#ARGS#${@:4}#' ./crab_data.cfg &> crab.cfg"

exit 0
