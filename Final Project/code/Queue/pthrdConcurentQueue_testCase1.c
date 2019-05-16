#include "threadsafelistenerqueue.c"
#include <iostream>
#include <omp.h>
#include <random>
#include <bits/stdc++.h>

using namespace std;

int NUM_THREADS = 1;
int QUEUE_SIZE  = 0;

// driver program to test above Concurrent Locked Queue
int main(int argc, char** argv){
  // Reading Program arguments to begin..
	QUEUE_SIZE = atoi(argv[1]);	// queue size
	NUM_THREADS = atoi(argv[2]); 	// num of threads
	 
	// Recording the time internally..
	double start = omp_get_wtime();
	ThreadSafeListenerQueue<int32_t> *concrntQueue = new ThreadSafeListenerQueue<int32_t>();
	int globalSumNQs = 0;
  	int globalSumDQs = 0;

	// TEST CASE - 1:
	// First all threads only Enqueue and in the second section all threads Dequeue concurrently from the Queue
        double enQTimeStart = omp_get_wtime();
	#pragma omp parallel for num_threads(NUM_THREADS) reduction(+: globalSumNQs)
        for(int i = 0; i < QUEUE_SIZE; i++){
            int thrd_id = omp_get_thread_num();
		 				int valToEnQ = (i+1);
            bool enqueued = concrntQueue->enqueue(valToEnQ);
            if(enqueued){
            	cout << "Thread# " << thrd_id << "  -- Enqueued = " << valToEnQ << endl;
              globalSumNQs += valToEnQ;
	    }
            else {
		cout << "Queue is now Full! Queue Size = " << QUEUE_SIZE << endl;
	    }
	}
	double enQTime = omp_get_wtime() - enQTimeStart;

	cout << endl;
	//Now, start Dequeing..
	double deQTimeStart = omp_get_wtime();
	#pragma omp parallel for num_threads(NUM_THREADS) reduction(+: globalSumDQs)
        for(int i = 0; i < QUEUE_SIZE; i++) {
	   int thrd_id = omp_get_thread_num();
	   int32_t valDequd = concrntQueue->dequeue();
	   if(-1 != valDequd){
          	cout << "Thread# " << thrd_id << "  -- Dequeued = " << valDequd << endl;
          globalSumDQs += valDequd;
    }
    else {
          cout << "Nothing to Dequeue. Queue is now Empty!" << endl;
	  }
	}
    	double deQTime = omp_get_wtime() - deQTimeStart;
	
	cout << " **************************************************************************** " <<endl;
	cout << " PTHREAD+MUTEX CONCURRENT QUEUE - TEST CASE - 1: " <<  endl;
	cout << " - All threads first only Enqueue in parallel and after all threads Dequeue concurrently from the Queue" << endl;
	cout << " - A running sum is calculated for both Enqueued Elements and then when those are Dequeued, " << endl;
	cout << " - The Sum must macth as equal iterations are given to bothe EnQueue and DeQueue equal to the Queue Size" << endl;
	
  	cout << endl;
	cout << endl;
	cout << " TEST CASE-1 RESULTS: " << endl;
	cout << "      N_QUEUE SUM IS: " << globalSumNQs << endl;
  	cout << "      D_QUEUE SUM IS: " << globalSumDQs << endl;
	cout << endl;
        if(globalSumNQs == globalSumDQs){
           cout << "Queue works correctly; N_QUEUE SUM equals D_QUEUE SUM!" << endl;
        }

        //Total time taken
        printf("Enqueue Time (omp_wtime): %f sec\n", enQTime);
	printf("Dequeue Time (omp_wtime): %f sec\n", deQTime);
        printf("Total Time   (omp_wtime): %f sec\n", omp_get_wtime() - start);
        cout << endl;
	cout << " **************************************************************************** " <<endl;
	cout << endl;

	return 0;
	}
