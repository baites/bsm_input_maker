#!/usr/bin/env bash

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

cp $1 $prod_folder/input.txt
cp $2 $prod_folder/cmssw_cfg.py

eval "sed -e 's#PWD#$PWD/$prod_folder#g' ./run.sh &> $prod_folder/run.sh"
eval "sed -e 's#PWD#$PWD/$prod_folder#g' ./condor.cfg &> $prod_folder/condor.cfg"
chmod u+x $prod_folder/run.sh

pushd $prod_folder &> /dev/null

condor_submit ./condor.cfg

popd &> /dev/null

echo done

exit 0
