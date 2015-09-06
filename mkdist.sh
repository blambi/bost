#!/bin/bash
VERSION="$( grep VERSION bost.h | sed -e 's:"$::' -e 's:^.*"::' )"
DISTNAME="bost-$VERSION"
make clean
mkdir ../$DISTNAME
for x in *; do
    if [ $x != "mkdist.sh" ]; then
        echo $x
        cp $x ../$DISTNAME/
    fi
done
tar -zcf ../$DISTNAME.tar.gz ../$DISTNAME
rm -R ../$DISTNAME
