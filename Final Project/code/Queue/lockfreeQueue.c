#include <bits/stdc++.h>
#include <iostream>
#include <omp.h>

#define default_value 16
using namespace std;

template<class T> class Queue{
	public:
	Queue(int = default_value); //default constructor
	~Queue()//destructor
	{ delete [] values; }
	bool enqueue(T);
        T dequeue();
        bool isEmpty();
        bool isFull();
	void print();

      private:
	int size;
	T *values;
        int front;
	int back;
};

template<class T>void Queue<T>::print(){
        cout << "Queue contains:: " << endl;
	for(int i=0; i < size; i++){
		cout << " At i = " << i << ", it is (" << values[i] << ")";
		cout << endl;
	}
	cout << "FRONT IS :: " << front <<  ", BACK IS :: " << back << endl;  
}

template<class T>Queue<T>::Queue(int x):
	size(x),//ctor
	values(new T[size]),
        front(0),
	back(0){}

template<class T>bool Queue<T>::isFull(){
	if((back+1) % size == front){
		return 1;
	}
	else {
		return 0;
	}	
}

template<class T>bool Queue<T>::enqueue(T x){
	bool enqueued = 0;
	if(!Queue<T>::isFull()){
		int tempBack = back;
		#pragma omp atomic capture
		{
	 	tempBack = back;
	 	//back = (back+1) % size;
	 	back+=1;
		}
		//cout << "Inside Enqueue -- BackOld = " << tempBack << ", BackNew = " << back << ", ValueToInsert = " << x << endl;
		values[tempBack] = x;
		enqueued = 1;
	}
	return enqueued;
}

template<class T>bool Queue<T>::isEmpty(){
	if(back == front){ return 1; }
    	else{ return 0; }
}

template<class T>T Queue<T>::dequeue(){
	T val = -1;
	if(!Queue<T>::isEmpty()){
		int tempFront = front;
	
		#pragma omp atomic capture
		{
		tempFront = front;
		front+=1;
		//front = (front+1) % size;
		}
	
		val = values[tempFront];
		//cout << "Inside Dequeue -- Dequeued from front = " << tempFront << ", ValueDequeued = " << val << " (Stop) " << endl;    
		}
	else {cerr << "Queue is Empty!";}
	return val;
}

