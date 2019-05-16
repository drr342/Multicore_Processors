//============================================================================
// Name        : lock_free.hpp
// Author      : Daniel Rivera
// Version     :
// Copyright   : Your copyright notice
// Description : Multicore Processors: Final Project
//============================================================================

#ifndef LOCKFREE_LINKED_LIST
#define LOCKFREE_LINKED_LIST

#include <iostream>
#include <atomic>
#include "list.h"

namespace LockFree 
{

template <typename T> 
class LinkedList : public LinkedListInterface<T> {
private:
	class Node {
	public:
		std::atomic<T> value;
		std::atomic<Node*> next;
		Node (const T& value = T(), Node *next = nullptr) {
			this->value = value;
			this->next = next;
		}
	};

	std::atomic<Node*> head, tail;
	std::atomic<int> sizeAtomic;
	void searchByValue (const T& value, Node **leftNode, Node **rightNode);
	bool searchByIndex (int index, Node **leftNode, Node **rightNode);
	bool addByIndex (int index, const T& value);
	T* removeByIndex (int index);
	T* setByIndex (int index, const T& value);
	T* getByIndex (int index) const;

	friend bool is_marked_reference (Node* n) {
		unsigned long v = reinterpret_cast<unsigned long>(n);
		return (v % 2) == 1;
	}
	friend Node* get_marked_reference (Node* n) {
	 	unsigned long v = reinterpret_cast<unsigned long>(n);
	    return reinterpret_cast<Node*>(v | 1);  // is ...00001
	}
	friend Node* get_unmarked_reference (Node* n) {
		unsigned long v = reinterpret_cast<unsigned long>(n);
		return reinterpret_cast<Node*>(v & -2); // is ..11110
	}
	
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
	// std::cout << "I am Lock Free :)" << std::endl;
	head = new Node();
	tail = new Node();
	head.load()->next.store(tail);
	sizeAtomic = 0;
}

template <typename T>
LinkedList<T>::~LinkedList () {
	Node *current = head;
	Node *next = get_unmarked_reference(current->next);
	while (next != nullptr) {
		delete current;
		current = next;
		next = get_unmarked_reference(current->next);
	}
	delete current;
}

template <typename T>
int LinkedList<T>::size () const {
	return sizeAtomic;
}

template <typename T>
bool LinkedList<T>::isEmpty () const {
	return (sizeAtomic == 0);
}

template <typename T>
int LinkedList<T>::indexOf (const T& value) const {
	Node *current = head.load()->next;
	int index = 0;
	while (current != tail) {
		if (!is_marked_reference(current->next)) {
			if (current->value == value)
				return index;
			index++;
		}
		current = (get_unmarked_reference(current))->next;
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
			if (!is_marked_reference(next)) {
				*left = current;
				leftNext = next;
			}
			current = get_unmarked_reference(next);
			if (current == tail) break;
			next = current->next;
		} while (is_marked_reference(next) || current->value != value);
				 // !( || current == tail));
		*right = current;
		// 2: Check nodes are adjacent
		if (leftNext == *right) {
			if ((*right != tail) && is_marked_reference((*right)->next))
				continue;
			else
				return;
		}
		// 3: Remove one or more marked nodes
		if (std::atomic_compare_exchange_strong(&((*left)->next), &leftNext, *right)) {
			delete leftNext;
			if ((*right != tail) && is_marked_reference((*right)->next))
				continue;
			else
				return;
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
			if (!is_marked_reference(next)) {
				*left = current;
				leftNext = next;
				currentIndex++;
			}
			current = get_unmarked_reference(next);
			// if (current == tail) break;
			next = current->next;
		} while ((is_marked_reference(next) || (index != currentIndex)) && 
				 (index != -1 || get_unmarked_reference(next) != tail) && current != tail);
		// check that we are at the right index
		if (index != -1 && index != currentIndex)
			return false;
		*right = current;
		// 2: Check nodes are adjacent
		if (leftNext == *right) {
			if ((*right != tail) && is_marked_reference((*right)->next))
				continue;
			else
				return true;
		}
		// 3: Remove one or more marked nodes
		if (std::atomic_compare_exchange_strong(&((*left)->next), &leftNext, *right)) {
			delete leftNext;
			if ((*right != tail) && is_marked_reference((*right)->next))
				continue;
			else
				return true;
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
		if (std::atomic_compare_exchange_strong(&(left->next), &right, newNode)) {
			sizeAtomic++;
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
		if (!is_marked_reference(rightNext)) {
			if (std::atomic_compare_exchange_strong(&(right->next), &rightNext, get_marked_reference(rightNext))) {
				sizeAtomic--;
				break;
			}
		}
	}
	// std::atomic_compare_exchange_strong(&(left->next), &right, rightNext);
	if (!std::atomic_compare_exchange_strong(&(left->next), &right, rightNext))
		searchByValue(right->value, &left, &right);
	else
		delete right;
	return true;
}

template <typename T>
T* LinkedList<T>::removeByIndex (int index) {
	Node *left, *right, *rightNext;
	while (true) {
		if(!searchByIndex(index, &left, &right))
			return nullptr;
		rightNext = right->next;
		if (!is_marked_reference(rightNext)) {
			if (std::atomic_compare_exchange_strong(&(right->next), &rightNext, get_marked_reference(rightNext))) {
				sizeAtomic--;
				break;
			}
		}
	}
	T && previous = right->value.load();
	if (!std::atomic_compare_exchange_strong(&(left->next), &right, rightNext))
		searchByValue(previous, &left, &right);
	else
		delete right;
	return &previous;
}

template <typename T>
T* LinkedList<T>::remove (int index) {
	if (index < 0 || index >= sizeAtomic || this->isEmpty())
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
	T && previous = reference->value.load();
	while (!std::atomic_compare_exchange_strong(&(reference->value), &previous, value)) {}
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
	Node *current = head.load()->next;
	T && value = current->value.load();
	int currentIndex = 0;
	while (current != tail) {
		if (!is_marked_reference(current->next)) {
			value = current->value.load();
			if (currentIndex == index)
				return &value;
			currentIndex++;
		}
		current = (get_unmarked_reference(current))->next;
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
	sizeAtomic.store(0);
}

template <typename T>
void LinkedList<T>::print () const {
	if (this->isEmpty()) {
		std::cout << "[]" << std::endl;
		return;
	}
	std::cout << '[';
	Node *current = head.load()->next;
	while (current != tail) {
		if (!is_marked_reference(current->next)) {
			std::cout << current->value << " ";
		}
		current = (get_unmarked_reference(current))->next;
	}
	std::cout << '\b' << ']' << std::endl;
}

}

#endif