#!/bin/sh

. ./mkstuff.conf

TARDIR=$PACKAGE-$VER
TARFILE=$PACKAGE-$VER.tar.gz

make dist
mkdir $TARDIR
cd $TARDIR
tar -xzvf ../gaim.tar.gz
rm -f ../gaim.tar.gz
cd ..
tar -czvf $TARFILE $TARDIR
rm -rf $TARDIR

echo
echo "TARBALL BUILT"
