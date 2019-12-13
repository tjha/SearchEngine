#!/bin/sh

if pgrep -x "driver.exe" > /dev/null
then
	kill -15 $(ps -e | grep "driver.exe" | grep -v grep | awk '{ print $1 }')
fi
crontab -r
