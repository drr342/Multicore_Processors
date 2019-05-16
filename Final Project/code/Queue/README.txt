
CONCURRENT QUEUE:

Following classes are used for the Concurrent Queue implementation:

1) OPENMP Locks Concurrent Queue:
   lockedqueue.c
   lockedQueue_testCase1.c  (main test class 1 for lockedqueue.c)
   lockedQueue_testCase2.c  (main test class 2 for lockedqueue.c)

2) OPENMP Lock-Free/Atomic Concurrent Queue:
   lockfreeQueue.c
   lockfreeQueue_testCase1.c (main test class 1 for lockfreeQueue.c)
   lockfreeQueue_testCase2.c (main test class 2 for lockfreeQueue.c)

3) C++ Pthreads/Mutex Concurrent Queue:
   threadsafelistenerqueue.c
   pthrdConcurentQueue_testCase1.c (main test class 1 for threadsafelistenerqueue.c)
   pthrdConcurentQueue_testCase2.c (main test class 2 for threadsafelistenerqueue.c)


Each Implementation has two main test cases:
TestCase1:
All threads first only Enqueue in parallel and after all threads Dequeue concurrently from the Queue. A running sum is calculated for both Enqueued Elements and then when those are Dequeued. The Sum must match as equal iterations are given to both EnQueue and DeQueue equal to the Queue Size.

TestCase2:
It runs both Enqueues and Dequeues concurrently in random order with roughly 50% probability of each operation. That is to test the Queue is growing and shrinking concurrently and is still consistent. It calculates a running sum of Enqueued and Dequeued elements. But, for this Test Case the two sums not necessarily need to be equal as there could be still elements left to dequeue because less number of iterations are given to the dequeue threads (as it is done  randomly).



Compile Instructions:

   g++ lockedQueue_testCase1.c -fopenmp -o lckdQueue_1
   g++ lockedQueue_testCase2.c -fopenmp -o lckdQueue_2
   g++ lockfreeQueue_testCase1.c -fopenmp -o lockfreeQueue_1
   g++ lockfreeQueue_testCase2.c -fopenmp -o lockfreeQueue_2
   g++ pthrdConcurentQueue_testCase1.c -fopenmp -o pthreadQueue_1
   g++ pthrdConcurentQueue_testCase2.c -fopenmp -o pthreadQueue_2

RUN INSTRUCTIONS:
   ./<executable_name> <QUEUE_SIZE> <NUM_THREADS>

   <QUEUE_SIZE> is a positive integer value
   <NUM_THREADS> is a positive integer value

Example:
   ./lckdQueue_1 1000 100
   ./lckdQueue_2  100 10
   ./lockfreeQueue_1  100 10
   ./lockfreeQueue_2  100 10
   ./pthreadQueue_1  100 100
   ./pthreadQueue_2  100 2



-----
