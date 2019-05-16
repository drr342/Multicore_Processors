//============================================================================
// Name        : lock_based.hpp
// Author      : Daniel Rivera
// Version     :
// Copyright   : Your copyright notice
// Description : Multicore Processors: Final Project
//============================================================================

#ifndef LOCKBASED_LINKED_LIST_H
#define LOCKBASED_LINKED_LIST_H

#include <iostream>
#include <omp.h>
#include "list.h"

namespace LockBased
{

template <typename T> 
class LinkedList : public LinkedListInterface<T> {
private:
	class Node {
	public:
		T value;
		Node* next;
		bool marked;
		omp_lock_t lock;
		Node (const T& value = T(), Node *next = nullptr) {
			this->value = value;
			this->next = next;
			this->marked = false;
			omp_init_lock(&lock);
		}
		~Node () {
			omp_set_lock(&lock);
			omp_unset_lock(&lock);
			omp_destroy_lock(&lock);
		}
	};

	Node *head, *tail;
	int sizeAttr;
	void searchByValue (const T& value, Node **leftNode, Node **rightNode);
	bool searchByIndex (int index, Node **leftNode, Node **rightNode);
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
	// std::cout << "I am Lock Based :)" << std::endl;
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
	return (sizeAttr == 0);
}

template <typename T>
int LinkedList<T>::indexOf (const T& value) const {
	Node *current = head->next;
	int index = 0;
	while (current != tail) {
		if (!(current->marked)) {
			if (current->value == value)
				return index;
			index++;
		}
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
void LinkedList<T>::searchByValue (const T& value, Node **left, Node **right) {
	Node *leftNext, *current, *next;
	while (true) {
		current = head;
		next = current->next;
		 // 1: Find leftNode and rightNode 
		do {
			if (!(current->marked)) {
				*left = current;
				leftNext = next;
			}
			current = next;
			if (current == tail) break;
			next = current->next;
		} while (current->marked || current->value != value);
				 // !( || current == tail));
		*right = current;
		// 2: Check nodes are adjacent
		if (leftNext == *right) {
			if ((*right != tail) && ((*right)->marked))
				continue;
			else
				return;
		}
		// 3: Remove one or more marked nodes
		if (omp_test_lock(&((*left)->lock))) {
			if ((*left)->next == leftNext) {
				(*left)->next = *right;
				delete leftNext;
				if ((*right != tail) && (*right)->marked){
					omp_unset_lock(&((*left)->lock));
					continue;
				}
				else {
					omp_unset_lock(&((*left)->lock));
					return;
				}
			}
			omp_unset_lock(&((*left)->lock));
		}
	}
}

template <typename T>
bool LinkedList<T>::searchByIndex (int index, Node **left, Node **right) {
	Node *leftNext, *current, *next;
	int currentIndex;
	while (true) {
		current = head;
		next = current->next;
		currentIndex = -1;
		// 1: Find leftNode and rightNode 
		do {
			if (!(current->marked)) {
				*left = current;
				leftNext = next;
				currentIndex++;
			}
			current = next;
			// if (current == tail) break;
			next = current->next;
		} while (((current != nullptr && current->marked) || (index != currentIndex)) && 
				 (index != -1 || next != tail) && current != tail);
		// check that we are at the right index
		if (index != -1 && index != currentIndex)
			return false;
		*right = current;
		// 2: Check nodes are adjacent
		if (leftNext == *right) {
			if ((*right != tail) && ((*right)->marked))
				continue;
			else
				return true;
		}
		// 3: Remove one or more marked nodes
		if (omp_test_lock(&((*left)->lock))) {
			if ((*left)->next == leftNext) {
				(*left)->next = *right;
				delete leftNext;
				if ((*right != tail) && (*right)->marked) {
					omp_unset_lock(&((*left)->lock));
					continue;
				}
				else {
					omp_unset_lock(&((*left)->lock));
					return true;
				}
			}
			omp_unset_lock(&((*left)->lock));
		}
	}
}

template <typename T>
bool LinkedList<T>::addByIndex (int index, const T& value) {
	Node *newNode = new Node(value);
	Node *left, *right;
	while (true) {
		if (!searchByIndex(index, &left, &right)) {
			delete newNode;
			return false;
		}
		newNode->next = right;
		if (omp_test_lock(&(left->lock))) {
			left->next = newNode;
			omp_unset_lock(&(left->lock));
			#pragma omp atomic
			sizeAttr++;
			return true;
		}
	}
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
	Node *left, *right, *rightNext;
	while (true) {
		searchByValue(value, &left, &right);
		if ((right == tail) || (right->value != value))
			return false;
		rightNext = right->next;
		if (!(right->marked)) {
			if (omp_test_lock(&(right->lock))) {
				right->marked = true;
				omp_unset_lock(&(right->lock));
				#pragma omp atomic
				sizeAttr--;
				break;
			}
		}
	}
	int deleted = 0;
	if (omp_test_lock(&(left->lock))) {
		if (left->next == right) {
			left->next = rightNext;
			delete right;
			deleted = 1;
		}	
		omp_unset_lock(&(left->lock));
	}
	if (!deleted)
		searchByValue(value, &left, &right);
	return true;
}

template <typename T>
T* LinkedList<T>::removeByIndex (int index) {
	Node *left, *right, *rightNext = nullptr;
	while (true) {
		if(!searchByIndex(index, &left, &right))
			return nullptr;
		rightNext = right->next;
		if (!(right->marked)) {
			if (omp_test_lock(&(right->lock))) {
				right->marked = true;
				omp_unset_lock(&(right->lock));
				#pragma omp atomic
				sizeAttr--;
				break;
			}
		}
	}
	T temp = right->value;
	T & previous = temp;
	int deleted = 0;
	if (omp_test_lock(&(left->lock))) {
		if (left->next == right) {
			left->next = rightNext;
			delete right;
			deleted = 1;
		}	
		omp_unset_lock(&(left->lock));
	}
	if (!deleted)
		searchByValue(previous, &left, &right);
	return &previous;
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
	Node *left, *right, *reference;
	if(!searchByIndex(index, &left, &right))
		return nullptr;
	reference = (index == -1) ? left : right;
	T temp = reference->value;
	T & previous = temp;
	omp_set_lock(&(reference->lock));
	reference->value = value;
	omp_unset_lock(&(reference->lock));
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
	Node *current = head->next;
	T temp = current->value;
	T & value = temp;
	int currentIndex = 0;
	while (current != tail) {
		if (!(current->marked)) {
			value = current->value;
			if (currentIndex == index)
				return &value;
			currentIndex++;
		}
		current = current->next;
	}
	if (index == -1)
		return &value;
	return nullptr;
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
	#pragma omp atomic
		sizeAttr *= 0;
}

template <typename T>
void LinkedList<T>::print () const {
	if (this->isEmpty()) {
		std::cout << "[]" << std::endl;
		return;
	}
	std::cout << '[';
	Node *current = head->next;
	while (current != tail) {
		if (!(current->marked))
			std::cout << current->value << " ";
		current = current->next;
	}
	std::cout << '\b' << ']' << std::endl;
}

}

#endif