#!/usr/bin/env bash

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
