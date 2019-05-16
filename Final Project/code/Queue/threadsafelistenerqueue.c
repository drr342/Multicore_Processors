#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <mutex>
#include <condition_variable>

using namespace std;

template <class T>
class ThreadSafeListenerQueue {
   private:
      std::list<T> thrdSfQueue;
      typename std::list<T>::iterator queue_itr;
      pthread_mutex_t mutex_queue_lock;
      pthread_cond_t cond_var;

   public:
      bool enqueue(const T element);
      T dequeue();
      bool listen(T& element);

      ThreadSafeListenerQueue() {    //Initializing Mutex + Condition Variable
        pthread_mutex_init(&mutex_queue_lock, NULL);
        pthread_cond_init(&cond_var, NULL);
      }
};

template <class T>
bool ThreadSafeListenerQueue<T>::enqueue(const T element) {
    pthread_mutex_lock(&mutex_queue_lock);

    thrdSfQueue.push_back(element);

    pthread_cond_signal(&cond_var);
    pthread_mutex_unlock(&mutex_queue_lock);

    return true;
}

template <class T>
T ThreadSafeListenerQueue<T>::dequeue() {
  T element = -1;
  pthread_mutex_lock(&mutex_queue_lock);
  if(!thrdSfQueue.empty()){
    element = thrdSfQueue.front();
    thrdSfQueue.pop_front();
  }
  pthread_mutex_unlock(&mutex_queue_lock);
  return element;
}

template <class T>
bool ThreadSafeListenerQueue<T>::listen(T& element) {
    bool poppedOff = false;
    pthread_mutex_lock(&mutex_queue_lock);

    while(thrdSfQueue.empty()){
      pthread_cond_wait(&cond_var, &mutex_queue_lock);
    }

    T elem = thrdSfQueue.front();
    thrdSfQueue.pop_front();
    element = elem;
    poppedOff = true;

    pthread_mutex_unlock(&mutex_queue_lock);
    return poppedOff;
}
