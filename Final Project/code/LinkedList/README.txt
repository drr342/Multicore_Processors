Multicore Processors: Architecture and Programming
Spring 2019

DANIEL RIVERA RUIZ and FATIMA MUSTAQ
drr342@nyu.edu         fm1529@nyu.edu 

Library for parallel data strucrtures: LINKED LIST

Source files:
	1) driver.cpp ------ Main method to run experiments on the linked list library
	2) list.h ---------- Interface to provide run-time polymorphism
	3) lock_based.hpp -- Lock based implementation of the linked list
	4) lock_free.hpp --- Locf free implementation of the linked list
	5) results.txt ----- Example output from test.sh
	6) sequeantial.hpp - Sequential (traditional) implementation of the linked list
	7) test.sh --------- Script to run several experiments using the driver

How to compile on CIMS machines:
	module purge
	module load gcc-8.2
	g++ -Wall -fopenmp -o driver driver.cpp

How to run:
	./driver <type> <threads> <repetitions>
		<type> = (b | f | s) for Lock-Based, Lock-Free or Sequential implementation
		<threads> = positive integer representing number of threads to use
		<repetitions> = positive integer representing how many times each operation will be called on the linked list

Observations:
	1) The sequential implementation of the linked list is not thread-safe, i.e. running experiments with more than one thread will yield erroneous results
	2) test.sh automatically runs the driver for experiments with 1,000 repetitions using 1,2,4,8,16,32,64 threads for lock-based and lock-free implementations
	3) The list is initialized by default to size 100,000. This parameter can only be modified in the source code of the driver (#define SIZE)
	4) By default each experiment will test all available APIs plus a final run where API calls are selected at random. To modify this behaviour, the source code of the driver must be modified (#define FIRST, #define LAST)