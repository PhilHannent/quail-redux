#!/bin/sh

# mkipk.sh
# generates an ipkg for Tickypip

# Based off the mkipk.sh by Bill Kendrick <bill@newbreedsoftware.com>

# Revised by Christian Hammond <chipx86@gnupdate.org>

. ./mkstuff.conf

DIST=$TMPDIR/BUILD/DIST
CONTROL=$DIST/CONTROL/control

echo "SETTING UP"
mkdir -p $TMPDIR/BUILD
mkdir -p $TMPDIR/BUILD/DIST/CONTROL

echo
echo "COPYING SOURCES INTO BUILD AREA"
cp -av src images levels tickypip.pro $TMPDIR/BUILD/

# Modify the tickypip.pro
perl -pi -e "s/debug/release/g" $TMPDIR/BUILD/tickypip.pro
perl -pi -e "s/LOCAL_COMPILE//g" $TMPDIR/BUILD/tickypip.pro

# Remove the xcf files
$RM $TMPDIR/BUILD/images/*.xcf
$RM -rf $TMPDIR/BUILD/images/.xvpics
$RM -rf $TMPDIR/BUILD/images/CVS
$RM -rf $TMPDIR/BUILD/levels/CVS
$RM -rf $TMPDIR/BUILD/levels/FORMAT*
$RM -rf $TMPDIR/BUILD/levels/*.pdb
$RM -rf $TMPDIR/BUILD/src/CVS
$RM -rf $TMPDIR/BUILD/CVS
cd $TMPDIR/BUILD

echo
echo "MAKING SURE BINARY EXISTS"
. $HOME/bin/dev-arm-qpe.sh
tmake tickypip.pro -o Makefile
make clean
make

if [ $? -ne 0 ]; then
	echo
	echo "ERROR IN COMPILE"
	exit 1
fi

cd ../..

echo 
echo "CREATING CONTROL FILE"

echo "Package: $PACKAGE" > $CONTROL
echo "Priority: optional" >> $CONTROL
echo "Version: $VER" >> $CONTROL
echo "Section: games" >> $CONTROL
echo "Architecture: $ARCH" >> $CONTROL
echo "Maintainer: Christian Hammond <chipx86@gnupdate.org>" >> $CONTROL
echo "Description: Connect the computers together to form an active network." >> $CONTROL

echo
echo "COPYING DATA FILES"

mkdir -p $DIST/opt/QtPalmtop/share/tickypip
mkdir $DIST/opt/QtPalmtop/share/tickypip/images/
cp -R $TMPDIR/BUILD/images/* $DIST/opt/QtPalmtop/share/tickypip/images/
mkdir -p $DIST/opt/QtPalmtop/share/tickypip/levels/
cp -R $TMPDIR/BUILD/levels/* $DIST/opt/QtPalmtop/share/tickypip/levels/

echo
echo "CREATING BINARIES"

mkdir -p $DIST/opt/QtPalmtop/bin/
cp $TMPDIR/BUILD/tickypip $DIST/opt/QtPalmtop/bin/

echo
echo "STRIPPING THE BINARY"
arm-linux-strip $DIST/opt/QtPalmtop/bin/tickypip

echo
echo "CREATING ICON AND DESKTOP FILE"

mkdir -p $DIST/opt/QtPalmtop/pics/
cp tickypip.png $DIST/opt/QtPalmtop/pics/tickypip.png

mkdir -p $DIST/opt/QtPalmtop/apps/Games/
DESKTOP=$DIST/opt/QtPalmtop/apps/Games/tickypip.desktop
echo "[Desktop Entry]" > $DESKTOP
echo "Comment=Connect the computers together to form an active network." >> $DESKTOP
echo "Exec=tickypip" >> $DESKTOP
echo "Icon=tickypip" >> $DESKTOP
echo "Type=Application" >> $DESKTOP
echo "Name=Tickypip" >> $DESKTOP


echo
echo "CREATING IPK..."

ipkg-build $DIST

echo
echo "CLEANING UP"

$RM -r $TMPDIR

echo

