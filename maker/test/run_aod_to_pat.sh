#!/usr/bin/env bash

if [[ 3 -gt $# ]]
then
    echo Usage: `basename $0` Cluster Process PNFS/Folder [cmssw args]

    exit 1
fi

cluster=$1
shift

process=$1
shift

pnfs_folder=$1/job.${cluster}.${process}
shift

mkdir /pnfs/cms/WAX/11/store/user/$pnfs_folder
if [[ 0 -ne $? ]]
then
    echo Failed to create output PNFS folder: $pnfs_folder

    exit 1
fi

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
getenv
echo
export
echo

cmsRun cmssw_cfg.py $@

export SRM_PATH=/opt/d-cache/srm
export X509_USER_PROXY=$PWD/proxy

for file in `find . -type f -name \*.root`
do
    /opt/d-cache/srm/bin/srmcp file:////$PWD/$file srm://cmssrm.fnal.gov:8443/srm/managerv1?SFN=/11/store/user/$pnfs_folder/$file

    rm $file
done

echo Output is saved in: $pnfs_folder

for file in cerr.log cout.log
do
    /opt/d-cache/srm/bin/srmcp file:////$PWD/$file srm://cmssrm.fnal.gov:8443/srm/managerv1?SFN=/11/store/user/$pnfs_folder/$file
done

exit 0
