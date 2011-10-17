#!/usr/bin/env bash

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
