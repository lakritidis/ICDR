/// ICDR: Indexed Contrastive Data Retriever

/// MaxHeap Implementation File: Max Heap with max-heapify. Used to maintain the best documents of
/// the query processing
///
/// Leonidas Akritidis, November 10th, 2011
/// ///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef ICDS_MAXHEAP_CPP
#define ICDS_MAXHEAP_CPP

#include "MaxHeap.h"

/// Initialize Binary heap to be able to accept MaxElements items.
MaxHeap::MaxHeap(int32_t MaxElements, int32_t thold) {

	/// Allocate memory for MaxElements items plus 1 for the sentinel.
	this->results = new Result *[MaxElements + 1];

	this->min_score = 0.0;
	this->threshold = thold;

	this->Capacity = MaxElements;
	this->Size = 0;
	this->alloc = 0;

	/// Allocate memory for the sentinel and place it in the Heap's head.
	this->results[0] = new Result();
	this->results[0]->set_score(1000000.0);
}

///  Heap Destructor
MaxHeap::~MaxHeap() {
/*
	for (uint32_t i = 1; i < this->Size + 1; i++) {
		delete this->results[i];
	}
*/
	delete this->results[0];
	delete [] this->results;
}

/// IsEmptyHeap: check whether the heap is empty
inline int32_t MaxHeap::is_empty () {
	return this->Size == 0;
}

/// IsFullHeap: check whether the heap is full.
inline int32_t MaxHeap::is_full () {
	return this->Size == this->Capacity;
}

/// Insert an item into the Binary Heap. The item is a HeapEntry structure having a
/// char (term) pointer and an integer (file, run that indicates the item's origin).
void MaxHeap::insert(class Result *res) {

	uint32_t i;

	/// If the Binary Heap is full, don't insert the item and return.
	if (this->is_full()) {
		printf("Priority queue is full\n");
		return;
	}

	/// Move the Heap's elements to preserve the max ordering attribute. Sort by score.
	for (i = ++this->Size; this->results[i / 2]->get_score() < res->get_score(); i /= 2) {
		this->results[i] = this->results[i / 2];
	}

	this->alloc++;
	this->results[i] = res;
}

/// Remove the Binary Heap's head, and permute the rest of the items to preserve the max-ordering.
/// Although the head is not deleted, the pointer that was pointing to the head now points to
/// another element (the new head). Thus, the returned value should be deleted outside the function
class Result *MaxHeap::remove_head() {

	uint32_t i, Child;
	class Result *MaxElement, *LastElement;

	/// If the Binary Heap is empty, remove nothing and return.
	if (this->is_empty()) {
		printf("Priority queue is empty");
		return this->results[0];
	}

	/// the minimal element resides in the Heap's head.
	MaxElement = this->results[1];
	LastElement = this->results[this->Size--];

	for (i = 1; i * 2 <= this->Size; i = Child) {
		/// We succesively find the maxima; element and its children.
		Child = i * 2;

		/// If the score is larger
		if (Child != this->Size && this->results[Child + 1]->get_score() > this->results[Child]->get_score()) {
			Child++;
		}

		/// If the score is smaller
		if (LastElement->get_score() < this->results[Child]->get_score()) {
			this->results[i] = this->results[Child];
		} else {
			break;
		}
	}
	this->results[i] = LastElement;

	return MaxElement;
}

#endif
