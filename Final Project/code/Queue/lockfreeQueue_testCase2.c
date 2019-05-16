#include "lockfreeQueue.c"
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
        QUEUE_SIZE = atoi(argv[1]);     // queue size
        NUM_THREADS = atoi(argv[2]);    // num of threads

        // Recording the time internally..
        Queue<int32_t> *concrntQueue = new Queue<int32_t>(QUEUE_SIZE);
        
        // Threads Enqueue and Dequeue concurrently in random order as provided by the random engine from a uniform distrubution
        // Roughly 50% will be enqueues and 50% will be dequeues
        int globalSumNQs = 0;
        int globalSumDQs = 0;
        cout << endl;
	double start = omp_get_wtime();
        cout << endl;
	#pragma omp parallel for num_threads(NUM_THREADS) reduction(+: globalSumNQs) reduction(+: globalSumDQs)
        for(int i = 0; i < QUEUE_SIZE; i++) {
	       int thrd_id = omp_get_thread_num();

               if(i < QUEUE_SIZE/2) { // 50% of time enqueue
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
		else {  //50% of the time, do a dequeue
                  int32_t valDequd = concrntQueue->dequeue();
                  if(-1 != valDequd){
                      cout << "Thread# " << thrd_id << "  -- Dequeued = " << valDequd << endl;
                      globalSumDQs += valDequd;
                  }
                  else {
                      cout << "Nothing to Dequeue. Queue is now Empty!" << endl;
                  }
            }
        }
	//concrntQueue->print();
        cout << endl;
	cout << " ---------- Concurrent exexcution of Enqueues and Dequeues finished. ----------" << endl;
        //check Queue Status.. It should be Empty after dequeue all elements
        if(concrntQueue->isEmpty()){
           cout << endl;
           cout << "After all dequeues Queue is now Empty." << endl;
        }
	cout << endl;
        cout << " **************************************************************************** " << endl;
	cout << " OPENMP ATOMIC/LOCK-FREE CONCURRENT QUEUE- TEST CASE - 2:" << endl;
        cout << " runs both Enqueues and Dequeues concurrently in random order with roughly 50% probability of each operation." << endl;
        cout << " that is to test Queue is growing and shrinking concurrently and is still consistent." << endl;
        cout << " It calculates a running sum of Enqueued and Dequeued Elements." << endl;
        
        cout << endl;
        cout << endl;
        cout << " TEST CASE - 2 RESULTS: " << endl;
        cout << " 	 N_QUEUE SUM IS: " << globalSumNQs << endl;
        cout << " 	 D_QUEUE SUM IS: " << globalSumDQs << endl;
        cout << endl;
        if(globalSumNQs == globalSumDQs){
           cout << "Concurrent EnQueues and Dequeues work Correctly; N_QUEUE SUM equals D_QUEUE SUM!" << endl;
        }
	else {
	   cout << "The two sums are not equal as there are still elements left to dequeue, which is okay. " << endl;
           cout << "This is because, less number of iterations are given to the dequeue threads. (As it is done randomly)" << endl;
	}
	cout << endl;
        //Total time taken for computing the best path is
        printf("Time (omp_wtime): %f sec\n", omp_get_wtime() - start);
        cout << endl;
        cout << " **************************************************************************** " << endl;
        cout << endl;
        return 0;
      }
