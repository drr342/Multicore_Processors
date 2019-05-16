#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <omp.h>

using namespace std;

template <class T>
class ConcurrentLockQueue {
   private:
      vector<T> queue;
      omp_lock_t q_lock;
      int capacity = 0;

      typename std::vector<T>::iterator itr;
      typename std::vector<T>::iterator begin();
      typename std::vector<T>::iterator end();

   public:
      bool enqueue(const T element);
      T* dequeue();
      T head();
      T tail();
      bool isEmpty();
      bool isFull();

      ConcurrentLockQueue() {
          capacity = INT_MAX;
          omp_init_lock(&q_lock);
      }

      ConcurrentLockQueue(int size) {
          capacity = size;
          omp_init_lock(&q_lock);
      }
};

template <class T>
T ConcurrentLockQueue<T>::head() {
  return queue.front();
}

template <class T>
T ConcurrentLockQueue<T>::tail() {
  return queue.back();
}

template <class T>
bool ConcurrentLockQueue<T>::isEmpty() {
  cout <<  "Queue Size: " << queue.size() << endl;
  return (queue.size() == 0 ? true : false);
}

template <class T>
bool ConcurrentLockQueue<T>::isFull() {
  return (capacity == queue.size() ? true : false);
}

template <class T>
bool ConcurrentLockQueue<T>::enqueue(const T element) {
  bool enqueued = false;
  if(!isFull()){
    omp_set_lock(&q_lock);
    queue.push_back(element);	
    omp_unset_lock(&q_lock);
    enqueued = true;
  }
  return enqueued;
}

template <class T>
T* ConcurrentLockQueue<T>::dequeue() {
  T* element = nullptr;
  if(!isEmpty()){
  	omp_set_lock(&q_lock);
	T elem = queue.front();
        element = new T(elem);        
	queue.erase(queue.begin());
        omp_unset_lock(&q_lock);
	//free(elemCopy);
	//return elemCopy;  
}
  //cout << "Inside Dequeue, Removed: " << *element << endl;
  return element;
}

template <class T>
typename std::vector<T>::iterator ConcurrentLockQueue<T>::begin() {
  return queue.begin();
}

template <class T>
typename std::vector<T>::iterator ConcurrentLockQueue<T>::end() {
  return queue.end();
}
