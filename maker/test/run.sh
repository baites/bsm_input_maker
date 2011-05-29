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

cp PWD/input.txt .
cp PWD/cmssw_cfg.py .
#/uscmst1/prod/sw/cms/slc5_amd64_gcc434/cms/cmssw/CMSSW_4_1_5/bin/slc5_amd64_gcc434/cmsRun cmssw_cfg.py
cmsRun cmssw_cfg.py

exit 0
