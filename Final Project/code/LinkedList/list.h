//============================================================================
// Name        : list.h
// Author      : Daniel Rivera
// Version     :
// Copyright   : Your copyright notice
// Description : Multicore Processors: Final Project
//============================================================================

#ifndef INTERFACE_LINKED_LIST
#define INTERFACE_LINKED_LIST

template <typename T> 
class LinkedListInterface {
public:
	virtual ~LinkedListInterface () {};
	virtual int size () const = 0;
	virtual bool isEmpty () const = 0;
	virtual int indexOf (const T& value) const = 0; // first occurrence
	virtual bool contains (const T& value) const = 0;
	virtual bool add (int index, const T& value) = 0;
	virtual bool addFirst (const T& value) = 0;
	virtual bool addLast (const T& value) = 0;
	virtual bool removeValue (const T& value) = 0; // first occurrence
	virtual T* remove (int index) = 0;
	virtual T* removeFirst () = 0;
	virtual T* removeLast () = 0;
	virtual T* set (int index, const T& value) = 0;
	virtual T* setFirst (const T& value) = 0;
	virtual T* setLast (const T& value) = 0;
	virtual T* get (int index) const = 0;
	virtual T* getFirst () const = 0;
	virtual T* getLast () const = 0;
	virtual void clear () = 0;
	virtual void print () const = 0;
};

#endif