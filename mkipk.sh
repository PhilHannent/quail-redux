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
mkdir -p $TMPDIR/opt/QtPalmtop/pics/gaim/protocols/small
mkdir -p $TMPDIR/opt/QtPalmtop/pics/gaim/status

$STRIP gaim

cp gaim.desktop $TMPDIR/opt/QtPalmtop/apps/Applications
cp gaim         $TMPDIR/opt/QtPalmtop/bin
cp gaim.png     $TMPDIR/opt/QtPalmtop/pics
cp data/images/*.png $TMPDIR/opt/QtPalmtop/pics/gaim
cp data/images/protocols/small/*.png $TMPDIR/opt/QtPalmtop/pics/gaim/protocols/small

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
Depends: qpe-base (\$QPE_VERSION), libgaim >= 0.67cvs-20030802, libglib2.0 | libglib2.0-0 | glib-2.0
Priority: optional
Section: Applications
Maintainer: Christian Hammond <chipx86@gnupdate.org>
Architecture: ${ARCHITECTURE}
License: GPL
Description: A multi-protocol instant messenger client, supporting AIM, ICQ, MSN, Yahoo!, Jabber, IRC, Zephyr, Gadu-Gadu, Napster, and Trepia.
END

ipkg-build $TMPDIR
rm -rf $TMPDIR
