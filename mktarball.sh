#!/bin/sh

. ./mkstuff.conf

TARDIR=$PACKAGE-$VER-$RELEASE
TARFILE=$PACKAGE-$VER-$RELEASE.tar.gz

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
