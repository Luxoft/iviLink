#!/bin/bash

BASENAME=`basename $0`

if [[ "$BASENAME" != "bash" ]]; then
	echo "Run this script as"
	echo ". ./`basename $0`"
	echo "Or, add proper command to ~/.bashrc"
	echo "For more information, see https://esg.luxoft.com/sen/wiki/display/AXIS/Linux+build+instructions#Linuxbuildinstructions-Buildonlypartofthetree%28runmakefromsubfolders%29"
fi

export AXIS_CONFIG_MK=`pwd`/config.mk
