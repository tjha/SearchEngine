#!/bin/sh

kill -15 $(ps -e | grep "driver.exe" | grep -v grep | awk '{ print $1 }')
cd $PATH_TO_SEARCH_ENGINE
make driver
./build/driver.exe > cronresult.txt &
crontab scripts/runOnCron.txt

