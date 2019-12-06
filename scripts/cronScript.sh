#!/bin/sh

kill -15 $(ps -e | grep "./build/driver.exe" | grep -v grep | awk '{ print $1 }')
sleep 5
cd $PATH_TO_SEARCH_ENGINE
./build/driver.exe
