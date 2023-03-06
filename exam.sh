#!/bin/bash
mkdir mydir
chmod 777  mydir
touch myfile
echo 2023 > myfile
mv moveme mydir
cp copyme mydir
cd mydir
mv copyme copied
cd ..
cat readme
gcc bad.c 2> err.txt
mkdir gen
cd gen
if [ $# -gt 0 ]
then
	a=1
	while [ $a -le $1 ]
	do
		touch $a.txt
		let a=a+1
	done
else
	b=1
	while [ $b -le 10 ]
	do 
		touch $b.txt
		let b=b+1
	done
fi
