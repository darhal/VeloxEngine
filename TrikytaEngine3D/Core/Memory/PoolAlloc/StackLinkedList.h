#pragma once
#include <Core/Misc/Defines/Common.hpp>
#include <stdio.h>

TRE_NS_START

template <class T>
class StackLinkedList {
public:
	struct Node {
		T m_Data;
		Node* m_Next;
	};
	Node* m_Head;
public:
	StackLinkedList() : m_Head(NULL) {};
	StackLinkedList(StackLinkedList& stackLinkedList) = delete;
	void  Push(Node * newNode);
	Node* Pop();
};

template <class T>
void StackLinkedList<T>::Push(Node* newNode) {
	newNode->m_Next = m_Head;
	m_Head = newNode;
}

template <class T>
typename StackLinkedList<T>::Node* StackLinkedList<T>::Pop() {
	Node* top = m_Head;
	m_Head = m_Head->m_Next;
	return top;
}

TRE_NS_END