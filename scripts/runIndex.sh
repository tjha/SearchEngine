#!/bin/bash

# jhirsh
# restart the indexerDriver script

batchName=~/someHtml/batch/
outputFolder=~/indexChunks3/

# watch -n 30 "if ps -e | grep indexerDriver; then ./build/indexerDriver.exe \
#	$batchName $outputFolder; fi \
#	find . -name '*.processed' | wc -l"

while true; do
	toProcess=$(find $batchName -name '*forIndexer' | wc -l)
	if [ $((toProcess)) -gt 0 ]; then
		./build/indexerDriver.exe $batchName $outputFolder
	else
		break
	fi
done
