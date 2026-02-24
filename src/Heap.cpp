/**
ICDR: Indexed Contrastive Data Retriever

Heap implementation file: Max/Min Heap with max/min-heapify. Used to maintain the best/worst
documents during query processing. Min Heap is used by BMW.

L. Akritidis, 2026
*/


#ifndef ICDS_MAXHEAP_CPP
#define ICDR_MAXHEAP_CPP

#include "Heap.h"

/// Initialize a Heap data structure with MaxElements capacity.
template <class T> Heap<T>::Heap(int32_t MaxElements) {

	/// Allocate memory for MaxElements items plus 1 for the sentinel.
	this->items = new Result *[MaxElements + 1];

	this->min_score = 0.0f;

	this->Capacity = MaxElements;
	this->Size = 0;
	this->alloc = 0;

	/// Allocate memory for the sentinel and place it in the Heap's head.
	this->items[0] = new Result();
}

/// Heap Destructor
template <class T> Heap<T>::~Heap() {
/*
	for (uint32_t i = 1; i < this->Size + 1; i++) {
		delete this->items[i];
	}
*/
	delete this->items[0];
	delete [] this->items;
}

/// IsEmptyHeap: Check whether the Heap is empty
template <class T> inline int32_t Heap<T>::is_empty () {
	return this->Size == 0;
}

/// IsFullHeap: Check whether the Heap is full.
template <class T> inline int32_t Heap<T>::is_full () {
	return this->Size == this->Capacity;
}

template <class T> inline T * Heap<T>::get_head() {
	return this->items[1];
}

/// ////////////////////////////////////////////////////////////////////////////////
/// MaxHeap Implementations : Derives from Heap. Keeps the max element in its head
template <class T> MaxHeap<T>::MaxHeap(int32_t MaxElements) : Heap<T>(MaxElements) {
		this->items[0]->set_score(1000000.0f);
}

/// Insert an item in the MaxHeap. The item is a T-type object that has a "score" member. After the
/// insertion, the structure stores the item with the maximum score in its head.
template <class T> void MaxHeap<T>::insert(T * itm) {
	uint32_t i = 0;

	/// If the  Heap is full, don't insert the item and return.
	if (this->is_full()) {
		printf("Priority queue is full\n");
		return;
	}

	/// Move the Heap's elements to preserve the max ordering attribute. Sort by score.
	for (i = ++this->Size; this->items[i / 2]->get_score() < itm->get_score(); i /= 2) {
		this->items[i] = this->items[i / 2];
	}

	this->alloc++;
	this->items[i] = itm;
}

/// Pop the element from the MaxHeap's head. Permute the rest of the items to preserve the ordering.
/// Although the head is not deleted, the pointer it was pointing to now points to another element.
template <class T> T * MaxHeap<T>::remove_head() {
	uint32_t i, Child;
	T * MaxElement, * LastElement;

	/// If the Binary Heap is empty, remove nothing and return.
	if (this->is_empty()) {
		printf("MaxHeap is empty");
		return this->items[0];
	}

	/// The maximum element resides in the Heap's head.
	MaxElement = this->items[1];
	LastElement = this->items[this->Size--];

	for (i = 1; i * 2 <= this->Size; i = Child) {
		/// We successively find the maximum; element and its children.
		Child = i * 2;

		/// If the score is larger
		if (Child != this->Size && this->items[Child + 1]->get_score() > this->items[Child]->get_score()) {
			Child++;
		}

		/// If the score is smaller
		if (LastElement->get_score() < this->items[Child]->get_score()) {
			this->items[i] = this->items[Child];
		} else {
			break;
		}
	}
	this->items[i] = LastElement;

	return MaxElement;
}

/// ////////////////////////////////////////////////////////////////////////////////
/// MinHeap Implementations : Derives from Heap. Keeps the min element in its head.
template <class T> MinHeap<T>::MinHeap(int32_t MaxElements) : Heap<T>(MaxElements) {
		this->items[0]->set_score(0.0f);
}

/// Insert an item in the MinHeap. The item is a T-type object that has a "score" member. After the
/// insertion, the structure stores the item with the maximum score in its head.
template <class T> void MinHeap<T>::insert(T * itm) {
	uint32_t i;

	// printf("\t\t\tINSERTING Cand DocID: %d. BM25 Score: %5.3f\n", itm->get_docID(), itm->get_score()); getchar();

	/// If the Heap is full, don't insert the item and return.
	if (this->is_full()) {
		printf("Priority queue is full\n");
		return;
	}

	/// Move the Heap's elements to preserve the min ordering attribute. Sort by score.
	for (i = ++this->Size; this->items[i / 2]->get_score() > itm->get_score(); i /= 2) {
		this->items[i] = this->items[i / 2];
	}

	this->alloc++;
	this->items[i] = itm;
}

/// Insert an item in the MinHeap with replacement. The item is a T-type object that has a "score"
/// member. The insertion takes place only if:
/// i) The heap is not full, and ii) if the score of the incoming element is greater than the min
/// in the Heap (stored in its head).
/// After the insertion, the structure stores the item with the maximum score in its head.
template <class T> void MinHeap<T>::insert_replace(score_t score, uint32_t docID, class Records * doc_info) {

	/// If the Heap is full, don't insert the item and return.
	if (this->is_full()) {
		if(this->items[1]->get_score() >= score) {
			return;
		} else {
			class Result * res = this->remove_head();
			delete res;
		}
	}

	T * itm = new Result(docID, doc_info->get_record(docID - 1)->get_text());
	itm->set_score(score);

	this->insert(itm);
}

/// Pop the element from the MinHeap's head. Permute the rest of the items to preserve the ordering.
/// Although the head is not deleted, the pointer it was pointing to now points to another element.
template <class T> T * MinHeap<T>::remove_head() {
	uint32_t i, Child;
	T * MinElement, * LastElement;

	/// If the Heap is empty, remove nothing and return.
	if (this->is_empty()) {
		printf("MinHeap is empty");
		return this->items[0];
	}

	/// The minimum element resides in the Heap's head.
	MinElement = this->items[1];
	LastElement = this->items[this->Size--];

	for (i = 1; i * 2 <= this->Size; i = Child) {
		/// We successively find the maximum; element and its children.
		Child = i * 2;

		/// If the score is smaller
		if (Child != this->Size && this->items[Child + 1]->get_score() < this->items[Child]->get_score()) {
			Child++;
		}

		/// If the score is larger
		if (LastElement->get_score() > this->items[Child]->get_score()) {
			this->items[i] = this->items[Child];
		} else {
			break;
		}
	}
	this->items[i] = LastElement;

	return MinElement;
}

#endif
