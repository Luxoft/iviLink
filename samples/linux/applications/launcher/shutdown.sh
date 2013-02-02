#!/bin/bash

cd ../bin/

kill -9 `pgrep IVILink`  2>/dev/null
kill -9 `pgrep AuthenticationApplication` 2>/dev/null
kill -9 `pgrep Climate_App` 2>/dev/null
kill -9 `pgrep Seat_App` 2>/dev/null
kill -9 `pgrep MediaApp` 2>/dev/null
kill -9 `pgrep Navi` 2>/dev/null
kill -9 `pgrep ScreenSharing` 2>/dev/null
kill -9 `pgrep launcher` 2>/dev/null

killall IVI*
killall Auth*
killall Clim*
killall Sea*
killall Med*
killall Nav*
killall Scr*
killall launcher*

pkill -9 IVI
pkill -9 Auth
pkill -9 Cli
pkill -9 Sea
pkill -9 Med
pkill -9 Nav
pkill -9 Scr
pkill -9 launcher

uuencode SystemController.log SystemController.log | mail -s Log -t knilivi@gmail.com
uuencode launcher.log launcher.log | mail -s Log -t knilivi@gmail.com

exit 0
