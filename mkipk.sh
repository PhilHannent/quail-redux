#!/bin/sh

. ./mkstuff.conf

if [ "$1" == "cross" ]; then
	export ARCHITECTURE=arm
	export STRIP=arm-linux-strip
elif [ "$1" == "native" ]; then
	export ARCHITECTURE=x86
	export STRIP=strip
else
	echo "usage: $0 cross | native [no-clean]"
	exit 1
fi

if [ "$2" != "no-clean" ]; then
	[ -e Makefile ] && make clean
	rm -f Makefile
fi

./make.sh $1

if [ $? -ne 0 ]; then
	echo
	echo "ERROR IN COMPILE"
	exit 1
fi

#lrelease gaim.pro

mkdir -p $TMPDIR/CONTROL
mkdir -p $TMPDIR/opt/QtPalmtop/apps/Applications
mkdir -p $TMPDIR/opt/QtPalmtop/bin
mkdir -p $TMPDIR/opt/QtPalmtop/pics
mkdir -p $TMPDIR/opt/QtPalmtop/pics/gaim
mkdir -p $TMPDIR/opt/QtPalmtop/pics/gaim/16x16
mkdir -p $TMPDIR/opt/QtPalmtop/pics/gaim/32x32
mkdir -p $TMPDIR/opt/QtPalmtop/pics/gaim/protocols
mkdir -p $TMPDIR/opt/QtPalmtop/pics/gaim/status

$STRIP gaim

cp gaim.desktop $TMPDIR/opt/QtPalmtop/apps/Applications
cp gaim         $TMPDIR/opt/QtPalmtop/bin
cp gaim.png     $TMPDIR/opt/QtPalmtop/pics
cp gaim.png     $TMPDIR/opt/QtPalmtop/pics/gaim
cp data/images/16x16/*.png $TMPDIR/opt/QtPalmtop/pics/gaim/16x16
cp data/images/32x32/*.png $TMPDIR/opt/QtPalmtop/pics/gaim/32x32
cp data/images/protocols/*.png $TMPDIR/opt/QtPalmtop/pics/gaim/protocols
cp data/images/status/*.png $TMPDIR/opt/QtPalmtop/pics/gaim/status

for LANG in $LANGUAGES
do
	mkdir -p $TMPDIR/opt/QtPalmtop/i18n/${LANG}

	if [ "$LANG" != "en" ]; then
		cp i18n/${LANG}/${PACKAGE}.qm $TMPDIR/opt/QtPalmtop/i18n/${LANG}
	fi
done

cat > $TMPDIR/CONTROL/control <<END
Package: ${PACKAGE}
Version: ${VER}-${RELEASE}
Depends: qpe-base (\$QPE_VERSION), libgaim >= 0.71
Priority: optional
Section: Applications
Maintainer: Christian Hammond <chipx86@gnupdate.org>
Architecture: ${ARCHITECTURE}
License: GPL
Description: A multi-protocol instant messenger client, supporting AIM, ICQ, MSN, Yahoo!, Jabber, IRC, Zephyr, Gadu-Gadu, Napster, and Trepia.
END

ipkg-build $TMPDIR
rm -rf $TMPDIR
