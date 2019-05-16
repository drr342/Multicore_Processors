#!/bin/bash

module purge
module load gcc-8.2

gcc -lm -g -Wall -fopenmp -o ptsm ptsm.c

for i in {1..5}
do
	echo "============= EXPERMINET $i ============="
	for j in {1..5}
	do
		echo "5 $j"
		time ./ptsm 5 $j cities5.txt
		echo ""
	done
	
	for j in {1..10}
	do
		echo "10 $j"
		time ./ptsm 10 $j cities10.txt
		echo ""
	done

done




