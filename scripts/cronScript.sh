#!/bin/sh

if pgrep -x "driver.exe" > /dev/null
then
	rm data/tmp/cronresult.txt
else
	rm data/tmp/cronresult.txt
	./build/driver.exe &
fi
