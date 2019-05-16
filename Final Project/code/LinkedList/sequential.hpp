//============================================================================
// Name        : sequential.hpp
// Author      : Daniel Rivera
// Version     :
// Copyright   : Your copyright notice
// Description : Multicore Processors: Final Project
//============================================================================

#ifndef SEQUENTIAL_LINKED_LIST
#define SEQUENTIAL_LINKED_LIST

#include <iostream>
#include "list.h"

namespace Sequential 
{

template <typename T> 
class LinkedList : public LinkedListInterface<T> {
private:
	class Node {
	public:
		T value;
		Node* next;
		Node (const T& value = T(), Node *next = nullptr) {
			this->value = value;
			this->next = next;
		}
	};
	Node *head, *tail;
	int sizeAttr;
	bool addByIndex (int index, const T& value);
	T* removeByIndex (int index);
	T* setByIndex (int index, const T& value);
	T* getByIndex (int index) const;
public:
	LinkedList ();
	~LinkedList ();
	int size () const;
	bool isEmpty () const;
	int indexOf (const T& value) const; // first occurrence
	bool contains (const T& value) const;
	bool add (int index, const T& value);
	bool addFirst (const T& value);
	bool addLast (const T& value);
	bool removeValue (const T& value); // first occurrence
	T* remove (int index);
	T* removeFirst ();
	T* removeLast ();
	T* set (int index, const T& value);
	T* setFirst (const T& value);
	T* setLast (const T& value);
	T* get (int index) const;
	T* getFirst () const;
	T* getLast () const;
	void clear ();
	void print () const;
};

template <typename T>
LinkedList<T>::LinkedList () {
	// std::cout << "I am Sequential :)" << std::endl;
	head = new Node();
	tail = new Node();
	head->next = tail;
	sizeAttr = 0;
}

template <typename T>
LinkedList<T>::~LinkedList () {
	Node *current = head;
	Node *next = current->next;
	while (next != nullptr) {
		delete current;
		current = next;
		next = current->next;
	}
	delete current;
}

template <typename T>
int LinkedList<T>::size () const {
	return sizeAttr;
}

template <typename T>
bool LinkedList<T>::isEmpty () const {
	return (head->next == tail);
}

template <typename T>
int LinkedList<T>::indexOf (const T& value) const {
	Node *current = head->next;
	int index = 0;
	while (current != tail) {
		if (current->value == value)
			return index;
		index++;
		current = current->next;
	}
	return -1;
}

template <typename T>
bool LinkedList<T>::contains (const T& value) const {
	if (this->indexOf(value) != -1)
		return true;
	else
		return false;
}

template <typename T>
bool LinkedList<T>::addByIndex (int index, const T& value) {
	Node *current = head;
	int currentIndex = 0;
	while (currentIndex != index && current->next != tail) {
		currentIndex++;
		current = current->next;
	}
	if (index != -1 && index != currentIndex)
		return false;
	Node *newNode = new Node(value, current->next);
	current->next = newNode;
	sizeAttr++;
	return true;
}

template <typename T>
bool LinkedList<T>::add (int index, const T& value) {
	if (index < 0)
		return false;
	return this->addByIndex(index, value);
}

template <typename T>
bool LinkedList<T>::addFirst (const T& value) {
	return this->addByIndex(0, value);
}

template <typename T>
bool LinkedList<T>::addLast (const T& value) {
	return this->addByIndex(-1, value);
}

template <typename T>
bool LinkedList<T>::removeValue (const T& value) {
	Node *previous = head;
	Node *current = head->next;
	while (current != tail) {
		if (current->value == value) {
			previous->next = current->next;
			delete current;
			sizeAttr--;
			return true;
		}
		previous = current;
		current = current->next;
	}
	return false;
}

template <typename T>
T* LinkedList<T>::removeByIndex (int index) {
	Node *previous = head;
	Node *current = head->next;
	int currentIndex = 0;
	while (currentIndex != index && current->next != tail) {
		currentIndex++;
		previous = current;
		current = current->next;
	}
	if (index != -1 && index != currentIndex)
		return nullptr;
	previous->next = current->next;
	T temp = current->value;
	T & previousValue = temp;
	delete current;
	sizeAttr--;
	return &previousValue;
}

template <typename T>
T* LinkedList<T>::remove (int index) {
	if (index < 0 || this->isEmpty())
		return nullptr;
	return this->removeByIndex(index);
}

template <typename T>
T* LinkedList<T>::removeFirst () {
	if (this->isEmpty())
		return nullptr;
	return this->removeByIndex(0);
}

template <typename T>
T* LinkedList<T>::removeLast () {
	if (this->isEmpty())
		return nullptr;
	return this->removeByIndex(-1);
}

template <typename T>
T* LinkedList<T>::setByIndex (int index, const T& value) {
	Node *reference, *current = head;
	int currentIndex = 0;
	while (currentIndex != index && current->next != tail) {
		currentIndex++;
		current = current->next;
	}
	if (index != -1 && index != currentIndex)
		return nullptr;
	reference = (index != -1) ? current->next : current;
	T temp = reference->value;
	T & previous = temp;
	reference->value = value;
	return &previous;
}

template <typename T>
T* LinkedList<T>::set (int index, const T& value) {
	if (index < 0 || this->isEmpty())
		return nullptr;
	return this->setByIndex(index, value);
}

template <typename T>
T* LinkedList<T>::setFirst (const T& value) {
	if (this->isEmpty())
		return nullptr;
	return this->setByIndex(0, value);
}

template <typename T>
T* LinkedList<T>::setLast (const T& value) {
	if (this->isEmpty())
		return nullptr;
	return this->setByIndex(-1, value);
}

template <typename T>
T* LinkedList<T>::getByIndex (int index) const {
	Node *reference, *current = head;
	int currentIndex = 0;
	while (currentIndex != index && current->next != tail) {
		currentIndex++;
		current = current->next;
	}
	if (index != -1 && index != currentIndex)
		return nullptr;
	reference = (index != -1) ? current->next : current;
	return &(reference->value);
}

template <typename T>
T* LinkedList<T>::get (int index) const {
	if (index < 0 || this->isEmpty())
		return nullptr;
	return this->getByIndex(index);
}

template <typename T>
T* LinkedList<T>::getFirst () const {
	if (this->isEmpty())
		return nullptr;
	return this->getByIndex(0);
}

template <typename T>
T* LinkedList<T>::getLast () const {
	if (this->isEmpty())
		return nullptr;
	return this->getByIndex(-1);
}

template <typename T>
void LinkedList<T>::clear () {
	while(removeFirst()) {}
	sizeAttr = 0;
}

template <typename T>
void LinkedList<T>::print () const {
	if (this->isEmpty()) {
		std::cout << "[]" << std::endl;
		return;
	}
	std::cout << '[';
	Node *current = this->head->next;
	while (current != this->tail) {
		std::cout << current->value << " ";
		current = current->next;
	}
	std::cout << '\b' << ']' << std::endl;
}

}

#endif