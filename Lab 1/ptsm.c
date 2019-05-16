/*
 *	DANIEL RIVERA RUIZ
 *	drr342@nyu.edu
 *	Multicore Processors
 *	Spring 2019
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <omp.h>

unsigned long int factorial (int n) {
	if (n == 0) return 1;
	return n * factorial(n - 1);
}

// A utility function to swap two characters a and b 
void swap (int* a, int* b) { 
	int t = *a; 
	*a = *b; 
	*b = t; 
} 

// This function finds the index of the smallest integer 
// which is greater than 'first' and is present in arr[l..h] 
int findCeil (int arr[], int first, int l, int h) { 
    // initialize index of ceiling element 
	int ceilIndex = l; 
    // Now iterate through rest of the elements and find 
    // the smallest character greater than 'first' 
	for (int i = l+1; i <= h; i++) {
		if (arr[i] > first && arr[i] < arr[ceilIndex]) 
			ceilIndex = i; 
	}
	return ceilIndex; 
}

// A utility function to reverse an array arr[l..h] 
void reverse(int arr[], int l, int h) { 
   while (l < h) { 
       swap(&arr[l], &arr[h]); 
       l++; 
       h--; 
   } 
} 

// Calculate the next permutation in ascending order
int * const nextPermutation (int * const previous, int size) {
	int i; 
    for (i = size - 2; i >= 0; i--) {
       if (previous[i] < previous[i+1]) 
          break; 
    }
    if (i == -1)
    	return previous;
    int ceilIndex = findCeil(previous, previous[i], i + 1, size - 1); 
    swap(&previous[i], &previous[ceilIndex]); 
    reverse(previous, i + 1, size - 1);
    return previous;
}

// calculate the total distance for a given path: stop calculation if it is greater than minDist
int getDistance (int * const weights, int * const path, int n, int minDist) {
	int distance = weights[path[0]];
	if (distance >= minDist) 
		return -1;
	for (int i = 0; i < n - 2; i++) {
		distance += weights[n * path[i] + path[i + 1]];
		if (distance >= minDist) 
			return -1;
	}
	return distance;
}

// calculate the factorial representation of a number: https://en.wikipedia.org/wiki/Factorial_number_system
int * const factorialRepresentation (unsigned long int n, int size) {
	int * const factorial = (int *) calloc(size, sizeof(int));
	for (int i = 1; i <= size; i++) {
		int q = n % i;
		n = n / i;
		factorial[size - i] = q;
	}
	return factorial;
}

// set the first path to search for the given perm_id
void setFirstPath(int * const path, unsigned long int perm_id, int size) {
	int * const factorialRep = factorialRepresentation(perm_id, size);
	int * const base = malloc(size * sizeof(int));
	for (int i = 0; i < size; i++) {
		base[i] = i + 1;
	}
	for (int i = 0; i < size; i++) {
		int target = factorialRep[i];
		int current = 0;
		while (1) {
			if (base[current] == -1) 
				target++; 
			if (current == target) {
				path[i] = base[current];
				base[current] = -1;
				break;
			}
			current++;
		}
	}
	free(factorialRep);
	free(base);
}

int main (int argc, char** argv) {

	// Check number of args is correct
	if(argc != 4) {
		fprintf(stderr, "Usage: ptsm x t citiesx.txt\n");
		fprintf(stderr, "x = number of cities\n");
		fprintf(stderr, "t = number of threads\n");
		fprintf(stderr, "citiesx.txt = text file with space-separated matrix of distances for the x cities\n");
		exit(1);
	}

	// open input file and check that it exists
	FILE * matrixFile = fopen(argv[3], "r");
	if (matrixFile == NULL) {
		fprintf(stderr, "Input file '%s' not found! Program will exit...\n", argv[2]);
		exit(1);
	}

	// allocate memory for matrix of weights
	int n = atoi(argv[1]);
	int * const weights = (int *) malloc(n * n * sizeof(int));

	// get number of total permutations
	unsigned long int perms = factorial(n - 1);

	// set number of threads: limit to number of permutations
	int t = atoi(argv[2]);
	if (t > perms) t = perms;
	// omp_set_num_threads(t);

	// populate matrix of weights
	int j = 0;
	int len = 16;
	char * line = (char *) malloc(len * sizeof(char));	
	for (int i = 0; i < n; i++) {
		long pos = ftell(matrixFile);
		fgets(line, len, matrixFile);
		// check that line is big enough to contain all data from file
		while (line[strlen(line)-1] != 10) {
			len *= 2;
			line = (char *) realloc(line, len * sizeof(char));
			fseek(matrixFile, pos, SEEK_SET);
			fgets(line, len, matrixFile);
		}
		// tokenize line and populate matrix of weights
		char * token = strtok(line, " ");
		while (*token != 10) {
			weights[j++] = atoi(token);
			token = strtok(NULL, " ");
		}
	}
	free(line);
	fclose(matrixFile);

	// start parallel code
	int thread_id, threads, dist, minDist;
	unsigned long int perm_id, perms_per_thread;
	int * path, * minPath;
	int global_minDist = INT_MAX;
	int * const global_minPath = (int *) malloc((n - 1) * sizeof(int));
	#pragma omp parallel \
		num_threads(t) \
		default(none) \
		private(thread_id, perm_id, path, minPath, dist, minDist) \
		shared(n, perms, threads, perms_per_thread, global_minDist)
	{
		// initialize shared vars
		#pragma omp single 
		{
			threads = omp_get_num_threads();
			perms_per_thread = (unsigned long int) ceil(perms / ((double) threads));
		}

		// initialize first possible path
		thread_id = omp_get_thread_num();
		perm_id = perms_per_thread * thread_id;
		path = (int *) calloc((n - 1), sizeof(int));
		setFirstPath(path, perm_id, n - 1);

		// initialize minDist and minPath
		minDist = getDistance(weights, path, n, INT_MAX);
		minPath = (int *) malloc((n - 1) * sizeof(int));
		memcpy(minPath, path, (n - 1) * sizeof(int));

		// traverse paths and calculate min distance
		for (int i = 1; i < perms_per_thread; i++) {
			path = nextPermutation(path, n - 1);
			dist = getDistance(weights, path, n, minDist);
			if (dist == -1) continue;
			minDist = dist;
			memcpy(minPath, path, (n - 1) * sizeof(int));
		}
		#pragma omp barrier

		// reduce min distance
		#pragma omp for \
			reduction(min: global_minDist)
		for (int i = 0; i < threads; i++) {
			if (global_minDist > minDist)
				global_minDist = minDist;
		}

		// set min path (non-deterministic if more than one)
		#pragma omp for
		for (int i = 0; i < threads; i++) {
			if (global_minDist == minDist)
				memcpy(global_minPath, minPath, (n - 1) * sizeof(int));
		}

		// free memory
		free(path);
		free(minPath);
	}

	printf("Shortest path is:\n0 ");
	for (int i = 0; i < n - 1; i++) {
		printf("%d ", global_minPath[i]);
	}
	printf("\ntotal weight = %d\n", global_minDist);

	free(weights);
	free(global_minPath);
	return 0;
}