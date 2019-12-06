#!/bin/sh

kill -15 $(ps -e | grep "driver.exe" | grep -v grep | awk '{ print $1 }')
crontab -r
