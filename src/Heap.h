/**
ICDR: Indexed Contrastive Data Retriever

Heap header file: Max/Min Heap with max/min-heapify. Used to maintain the best/worst documents
during query processing. Min Heap is used by BMW.

L. Akritidis, 2026
*/

#ifndef ICDR_MAXHEAP_H
#define ICDR_MAXHEAP_H

template<class T> class Heap {
	protected:
    	T ** items;

		double min_score;

		uint32_t Capacity;
		uint32_t Size;
		uint32_t alloc;

	public:
		Heap(int);
		~Heap();
		int32_t is_empty();
		int32_t is_full();
		T * get_head();
};

template<class T> class MaxHeap : public Heap<T> {
	public:
		MaxHeap(int);
		void insert(T *);
		T * remove_head();
};

template<class T> class MinHeap : public Heap<T> {
	public:
		MinHeap(int);
		void insert(T *);
		//void insert_replace(T *);
		void insert_replace(score_t, uint32_t, class Records *);
		T * remove_head();
};

#endif
