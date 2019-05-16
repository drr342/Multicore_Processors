//============================================================================
// Name        : driver.cpp
// Author      : Daniel Rivera
// Version     :
// Copyright   : Your copyright notice
// Description : Multicore Processors: Final Project
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <omp.h>
#include "list.h"
#include "lock_free.hpp"
#include "lock_based.hpp"
#include "sequential.hpp"

#define SIZE  100000 // Initial size of the list
#define FIRST 0      // First API to call in the experiment (as per the order below)
#define LAST  15     // Last API to call in the experiment (as per the order below)

LinkedListInterface<int> *list;

// Standardized functions for array of function pointers
void indexOf     (int index, int value) {list->indexOf     (value);}
void contains    (int index, int value) {list->contains    (value);}
void add         (int index, int value) {list->add  (index, value);}
void addFirst    (int index, int value) {list->addFirst    (value);}
void addLast     (int index, int value) {list->addLast     (value);}
void removeValue (int index, int value) {list->removeValue (value);}
void remove      (int index, int value) {list->remove      (index);}
void removeFirst (int index, int value) {list->removeFirst     ( );}
void removeLast  (int index, int value) {list->removeLast      ( );}
void set         (int index, int value) {list->set  (index, value);}
void setFirst    (int index, int value) {list->setFirst    (value);}
void setLast     (int index, int value) {list->setLast     (value);}
void get         (int index, int value) {list->get         (index);}
void getFirst    (int index, int value) {list->getFirst        ( );}
void getLast     (int index, int value) {list->getLast         ( );}

// Global arrays: function pointers, names and times
void (*functions[15]) (int index, int value);
const char *func_names[15];
double func_times[15];

int main (int argc, char **argv) {

	// Parse args
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <type> <threads> <repetitions>\n", argv[0]);
		return 1;
	}
	char type = argv[1][0];
	if (type != 'b' && type != 'f' && type != 's') {
		fprintf(stderr, "Type must be one of 'b', 'f' or 's' (lock-Based, lock-Free, Sequential)\n");
		return 1;
	}
	int t = atoi(argv[2]);
	if (t <= 0) {
		fprintf(stderr, "Number of threads must be a positive integer\n");
		return 1;
	}
	int n = atoi(argv[3]);
	if (n <= 0) {
		fprintf(stderr, "Number of repetitions must be a positive integer\n");
		return 1;
	}

	// Create list
	const char *type_str;
	switch (type) {
		case 'b':
			list = new LockBased::LinkedList<int>();
			type_str = "Lock-Based";
			break;
		case 'f':
			list = new LockFree::LinkedList<int>();
			type_str = "Lock-Free";
			break;
		case 's':
			list = new Sequential::LinkedList<int>();
			type_str = "Sequential";
			break;
	}
	
	// Populate list
	for (int i = 0; i < SIZE; i++)
		list->addFirst(i);

	// Set function pointers
	functions[0] = indexOf;
	functions[1] = contains;
	functions[2] = add;
	functions[3] = addFirst;
	functions[4] = addLast;
	functions[5] = removeValue;
	functions[6] = remove;
	functions[7] = removeFirst;
	functions[8] = removeLast;
	functions[9] = set;
	functions[10] = setFirst;
	functions[11] = setLast;
	functions[12] = get;
	functions[13] = getFirst;
	functions[14] = getLast;

	// Set function names
	func_names[0] = "indexOf";
	func_names[1] = "contains";
	func_names[2] = "add";
	func_names[3] = "addFirst";
	func_names[4] = "addLast";
	func_names[5] = "removeValue";
	func_names[6] = "remove";
	func_names[7] = "removeFirst";
	func_names[8] = "removeLast";
	func_names[9] = "set";
	func_names[10] = "setFirst";
	func_names[11] = "setLast";
	func_names[12] = "get";
	func_names[13] = "getFirst";
	func_names[14] = "getLast";

	// Perform each function n times using t threads
	srand(1234);
	double t_start, t_all = omp_get_wtime();
	for (int i = FIRST; i < LAST; i++) {
		t_start = omp_get_wtime();
		#pragma omp parallel for num_threads(t) default(none) shared(list, t, n, i, functions)
		for (int j = 0; j < n; j++)
			(*functions[i])(rand() % SIZE, rand() % SIZE);
		func_times[i] = omp_get_wtime() - t_start;
	}
	// Perform n functions (selected randomly) using t threads
	double t_random = omp_get_wtime();
	#pragma omp parallel for num_threads(t) default(none) shared(list, t, n, functions)
	for (int j = 0; j < n; j++)
		(*functions[rand() % 15])(rand() % SIZE, rand() % SIZE);
	t_random = omp_get_wtime() - t_random;
	t_all = omp_get_wtime() - t_all;

	// Print results of the experiment
	printf("============================\n");
	printf("|           RESULTS        |\n");
	printf("============================\n");
	printf("|%-15s|%10s|\n", "Implementation", type_str);
	printf("|%-15s|%10d|\n", "Threads", t);
	printf("|%-15s|%10d|\n", "Repetitions", n);
	printf("|--------------------------|\n");
	printf("|%-15s|%10s|\n", "OPERATION", "TIME (s)");
	printf("|--------------------------|\n");
	for (int i = FIRST; i < LAST; i++)
		printf("|%-15s|%10.4lf|\n", func_names[i], func_times[i]);
	printf("|--------------------------|\n");
	printf("|%-15s|%10.4lf|\n", "Random", t_random);
	printf("|--------------------------|\n");
	printf("|%-15s|%10.4lf|\n", "TOTAL", t_all);
	printf("============================\n");

	delete list;
	return 0;
}