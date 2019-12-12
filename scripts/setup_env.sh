#!/bin/bash

# Setup work environment for eecs398 - Search Enginge
# run the script using source ( . ~/dotfiles/eecs398_env.sh )
# jhirsh

#cd ~/Desktop/eecs398/SearchEngine
tmux new-session -s eecs398 \; \
	set-option remain-on-exit on \; \
	split-window -h 'sleep 5; cd data/tmp/logs; a=$(ls -At | head -n 1); tail -f $a; exec bash' \; \
	split-window -t 0 -v -l 13 'make clean; make cleanDriver; make driver; ./build/driver.exe; exec bash' \; \
	split-window -t 2 -v -l 3 'watch -n 10 wc -l data/crawledLinks.txt; exec bash'  \;

