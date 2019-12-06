#!/bin/sh

if pgrep -x "driver.exe" > /dev/null
then
	echo Running!
else
	cd $PATH_TO_SEARCH_ENGINE
	./build/driver.exe &
fi
