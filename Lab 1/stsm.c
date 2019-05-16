#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// unsigned long int factorial (int n) {
// 	if (n == 0) return 1;
// 	return n * factorial(n - 1);
// }

// int compare (const void *a, const void * b) {
// 	return ( *(int *)a - *(int *)b ); 
// } 

// A utility function two swap two characters a and b 
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

int * const nextPermutation (int * const previous, int size) {
	int i; 
    for (i = size - 2; i >= 0; i--) {
       if (previous[i] < previous[i+1]) 
          break; 
    }
    if (i == -1) {
    	free(previous);
    	return NULL;
    }
    int ceilIndex = findCeil(previous, previous[i], i + 1, size - 1); 
    swap(&previous[i], &previous[ceilIndex]); 
    reverse(previous, i + 1, size - 1);
    return previous;
}

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

int main (int argc, char** argv) {

	// Check number of args is correct
	if(argc != 3) {
		fprintf(stderr, "Usage: stsm x citiesx.txt\n");
		fprintf(stderr, "x = number of cities\n");
		fprintf(stderr, "citiesx.txt = text file with space-separated matrixFile of distances for the x cities\n");
		exit(1);
	}

	// open input file and check that it exists
	FILE * matrixFile = fopen(argv[2], "r");
	if (matrixFile == NULL) {
		fprintf(stderr, "Input file '%s' not found! Program will exit...\n", argv[2]);
		exit(1);
	}

	// allocate memory for matrix of weights
	int n = atoi(argv[1]);
	int * const weights = (int *) malloc(n * n * sizeof(int));

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

	// initialize first possible path (ascending order)
	int * path = (int *) malloc((n - 1) * sizeof(int));
	int * minPath = (int *) malloc((n - 1) * sizeof(int));
	for (int i = 0; i < n - 1; i++) {
		path[i] = i + 1;
	}
	int minDist = getDistance(weights, path, n, INT_MAX);
	memcpy(minPath, path, (n - 1) * sizeof(int));

	// traverse paths and calculate min distance
	while (path = nextPermutation(path, n-1)) {
		int dist = getDistance(weights, path, n, minDist);
		if (dist == -1) continue;
		minDist = dist;
		memcpy(minPath, path, (n - 1) * sizeof(int));
	}

	// print optimal path and distance
	printf("Shortest path is:\n0 ");
	for (int i = 0; i < n-1; i++) {
		printf("%d ", minPath[i]);
	}
	printf("\ntotal weight = %d\n", minDist);

	free(path);
	free(minPath);
	free(weights);
	return 0;
}