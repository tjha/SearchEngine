#!/bin/sh

cd $PATH_TO_SEARCH_ENGINE
if pgrep -x "driver.exe" > /dev/null
then
	rm scripts/tmp/cronresult.txt
else
	rm scripts/tmp/cronresult.txt
	./build/driver.exe &
fi
