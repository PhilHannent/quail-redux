#!/bin/sh

# mkipk.sh
# generates an ipkg for qpe-gaim

# Based off the mkipk.sh by Bill Kendrick <bill@newbreedsoftware.com>

# Revised by Christian Hammond <chipx86@gnupdate.org>

if [ ! -e $HOME/bin/dev-arm-qpe.sh ]; then
	echo "$HOME/bin/dev-arm-qpe.sh is not present."
	echo "This is required for setting the ARM development environment "
	echo "variables."
	exit 1
fi

. ./mkstuff.conf

DIST=$TMPDIR/BUILD/DIST
CONTROL=$DIST/CONTROL/control

echo "SETTING UP"
mkdir -p $TMPDIR/BUILD
mkdir -p $TMPDIR/BUILD/DIST/CONTROL

echo
echo "COPYING SOURCES INTO BUILD AREA"
cp -av src data gaim.pro $TMPDIR/BUILD/

# Modify the gaim.pro
perl -pi -e "s/debug/release/g" $TMPDIR/BUILD/gaim.pro
perl -pi -e "s/LOCAL_COMPILE//g" $TMPDIR/BUILD/gaim.pro

# Remove the xcf files
$RM -f $TMPDIR/BUILD/images/*.xcf
$RM -rf $TMPDIR/BUILD/data/.cvsignore
$RM -rf $TMPDIR/BUILD/data/CVS
$RM -rf $TMPDIR/BUILD/data/images/.cvsignore
$RM -rf $TMPDIR/BUILD/data/images/.xvpics
$RM -rf $TMPDIR/BUILD/data/images/CVS
$RM -rf $TMPDIR/BUILD/data/images/protocols/.cvsignore
$RM -rf $TMPDIR/BUILD/data/images/protocols/.xvpics
$RM -rf $TMPDIR/BUILD/data/images/protocols/CVS
$RM -rf $TMPDIR/BUILD/data/images/protocols/small/.cvsignore
$RM -rf $TMPDIR/BUILD/data/images/protocols/small/.xvpics
$RM -rf $TMPDIR/BUILD/data/images/protocols/small/CVS
$RM -rf $TMPDIR/BUILD/data/images/status/.cvsignore
$RM -rf $TMPDIR/BUILD/data/images/status/.xvpics
$RM -rf $TMPDIR/BUILD/data/images/status/CVS
$RM -rf $TMPDIR/BUILD/src/CVS
$RM -rf $TMPDIR/BUILD/CVS
cd $TMPDIR/BUILD

echo
echo "MAKING SURE BINARY EXISTS"
. $HOME/bin/dev-arm-qpe.sh
tmake gaim.pro -o Makefile
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
echo "Section: qpe/applications" >> $CONTROL
echo "Architecture: $ARCH" >> $CONTROL
echo "Maintainer: Christian Hammond <chipx86@gnupdate.org>" >> $CONTROL
echo "Depends: libglib2.0, libgaim"
echo "Description: A multi-protocol instant messenger client, supporting AIM, ICQ, MSN, Yahoo!, Jabber, IRC, Zephyr, Gadu-Gadu, Napster, and Trepia." >> $CONTROL

echo
echo "COPYING DATA FILES"

mkdir -p $DIST/opt/QtPalmtop/share/gaim
cp -R $TMPDIR/BUILD/data/* $DIST/opt/QtPalmtop/share/gaim/

echo
echo "CREATING BINARIES"

mkdir -p $DIST/opt/QtPalmtop/bin/
cp $TMPDIR/BUILD/gaim $DIST/opt/QtPalmtop/bin/

echo
echo "STRIPPING THE BINARY"
arm-linux-strip $DIST/opt/QtPalmtop/bin/gaim

echo
echo "CREATING ICON AND DESKTOP FILE"

mkdir -p $DIST/opt/QtPalmtop/pics/
cp gaim.png $DIST/opt/QtPalmtop/pics/gaim.png

mkdir -p $DIST/opt/QtPalmtop/apps/Applications/
DESKTOP=$DIST/opt/QtPalmtop/apps/Applications/gaim.desktop
echo "[Desktop Entry]" > $DESKTOP
echo "Comment=A multi-protocol instant messenger client." >> $DESKTOP
echo "Exec=gaim" >> $DESKTOP
echo "Icon=gaim" >> $DESKTOP
echo "Type=Application" >> $DESKTOP
echo "Name=Gaim" >> $DESKTOP


echo
echo "CREATING IPK..."

ipkg-build $DIST

echo
echo "CLEANING UP"

$RM -r $TMPDIR

echo

