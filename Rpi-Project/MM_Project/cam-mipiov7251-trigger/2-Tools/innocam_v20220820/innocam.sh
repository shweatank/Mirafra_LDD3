#!/bin/sh
appname=`basename $0 | sed s,\.sh$,,`
dirname=$PWD

strArch=$(arch)

case $strArch in
	*armv7l*)
		echo "System is 32 bit arch."
		appname=$appname"32"
		LD_LIBRARY_PATH=$dirname/lib32
	;;
	*aarch64*)
		arch=arm64
		echo "System is 64 bit arch."
		appname=$appname"64"
		LD_LIBRARY_PATH=$dirname/lib64
	;;
	*)
		echo "@ Unkown System Arch! Please contact support@inno-maker.com"
		exit
	;;
esac

export LD_LIBRARY_PATH
echo "LD_LIBRARY_PATH="$LD_LIBRARY_PATH
echo "appname="$appname
echo "dirname="$dirname

sudo $dirname/$appname "$@"
