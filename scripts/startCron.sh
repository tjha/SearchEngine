#!/bin/sh

PATH_TO_SEARCH_ENGINE=$(pwd)
kill -15 $(ps -e | grep "driver.exe" | grep -v grep | awk '{ print $1 }')
cd $PATH_TO_SEARCH_ENGINE
./build/driver.exe > cronresult.txt &
crontab scripts/runOnCron.txt

