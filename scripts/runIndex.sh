#!/bin/bash

# jhirsh
# restart the indexerDriver script

batchName=~/lougheeds_fun_test_directory/
outputFolder=~/mediumIndexChunks/

watch -n 30 "if ps -e | grep indexerDriver; then ./build/indexerDriver.exe $batchName $outputFolder; fi"

