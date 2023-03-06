#!/bin/bash

if [ $# -eq 1 ];
then
    grep -E "WARN.*|ERROR.*" $1 > bug.txt
 
else
    case $2 in
    "--latest")
        # Your code here. (2/4)
    tail -n 5 $1     
    ;;
    "--find")
        # Your code here. (3/4)
    grep $3 $1 > $3.txt 
    ;;
    "--diff")
    diff -s  $1 $3 1> /dev/null
    a=0
	if [ $a -eq 0 ]
        then
		echo same
	else
		echo different
	fi
    ;;
    esac
fi
