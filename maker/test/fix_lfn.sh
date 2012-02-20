#!/usr/bin/env bash
#
# Add leading slash to CRAB LFNs in XMLs
#
# Created by Samvel Khalatyan, Oct 17, 2011
# Copyright 2011, All rights reserved

if [[ 1 > $# ]]
then
    echo "usage: $0 crab_folder"

    exit 0
fi

pushd $1/res
for file in `find . -maxdepth 1 -type f -name \*.xml`
do
    backup=`basename $file .xml`_backup.xml
    mv $file $backup
    sed -e 's#\(\s\+\)store#\1/store#' $backup > $file
done

popd

exit 1
