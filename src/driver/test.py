#!/usr/bin/python
import subprocess

with open('result.txt', 'w') as outfile:
	for x in range(0,1000):
		subprocess.check_call("./driver.exe", stdout = outfile)