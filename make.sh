. ./mkstuff.conf

if [ "$1" = "cross" ]; then
	export CROSSCOMPILE=/opt/Embedix/tools
	export QPEDIR=/opt/Qtopia/sharp
	export QTDIR=/opt/Qtopia/sharp
	export PATH=$QTDIR/bin:$QPEDIR/bin:$CROSSCOMPILE/bin:$PATH
	export TMAKEPATH=/opt/Qtopia/tmake/lib/qws/linux-sharp-g++/
	export LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH
	export ARCHITECTURE=arm
elif [ "$1" = "native" ]; then
	export CROSSCOMPILE=
	export QPEDIR=/opt/Qtopia
	export QTDIR=/opt/Qtopia
	export PATH=$QTDIR/bin:$QPEDIR/bin:$PATH:/opt/Embedix/tools/bin
	export TMAKEPATH=/opt/Qtopia/tmake/lib/qws/linux-x86-g++/
	export LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH
	export ARCHITECTURE=x86
	export EXTRA_FLAGS="DEFINES+=LOCAL_COMPILE"
else
	echo "usage: $0 cross | native"
	exit 1
fi

tmake $EXTRA_FLAGS -o Makefile gaim.pro
make
