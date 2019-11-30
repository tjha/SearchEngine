#!/bin/bash

# Setup work environment for eecs398 - Search Enginge
# run the script using source ( . ~/dotfiles/eecs398_env.sh )
# jhirsh

cd ~/Desktop/eecs398/SearchEngine
tmux new-session -s eecs398 \; \
	set-option remain-on-exit on \; \
	split-window -h 'sleep 2; cd src/mvp/logs; ls -At | tail -f; exec bash' \; \
	split-window -t 0 -v -l 13 'cd src/mvp; make clean; make; ./driver.exe; exec bash' \; \
	split-window -t 2 -v -l 20 'htop' \;

