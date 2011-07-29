#!/usr/bin/env bash

if [[ 2 -gt $# ]]
then
    echo Usage: `basename $0` Cluster Process [cmssw args]

    exit 1
fi

cluster=$1
shift

process=$1
shift

wget http://home.fnal.gov/~samvel/protobuf-2.3.0.tar.bz2
tar -xjf protobuf-2.3.0.tar.bz2
rm protobuf-2.3.0.tar.bz2

run_folder=`pwd`

mkdir proto

pushd protobuf-2.3.0
./configure --prefix=$run_folder/proto

make
make install

popd

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$run_folder/proto/lib

tree

echo
echo $LD_LIBRARY_PATH
echo

cmsRun cmssw_cfg.py $@

exit 0
