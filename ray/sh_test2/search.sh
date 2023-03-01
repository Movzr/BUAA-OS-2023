#!/bin/bash
#First you can use grep (-n) to find the number of lines of string.
#Then you can use awk to separate the answer.
grep -n int $1|awk -F: '{print $1}' > $2
