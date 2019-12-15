#!/bin/bash

# jhirsh
# restart the indexerDriver script

batchName=../matthew-batch0
outputFolder=indexChunks

watch -n 30 "if ps -e | grep -q indexerDriver; then ./build/indexerDriver.exe $batchName $outputFolder; fi"

