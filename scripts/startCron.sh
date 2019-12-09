#!/bin/sh

if pgrep -x "driver.exe" > /dev/null
then
	kill -15 $(ps -e | grep "driver.exe" | grep -v grep | awk '{ print $1 }')
fi
cd $PATH_TO_SEARCH_ENGINE
make driver
mkdir -p data/tmp
./build/driver.exe > data/tmp/cronresult.txt &
crontab scripts/runOnCron.txt
