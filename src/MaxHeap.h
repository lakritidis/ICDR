/// ICDR: Indexed Contrastive Data Retriever

/// MaxHeap Header File: Max Heap with max-heapify. Used to maintain the best documents of
/// the query processing.
/// Leonidas Akritidis, November 10th, 2011
/// ///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef ICDS_MAXHEAP_H
#define ICDS_MAXHEAP_H

class MaxHeap {
	private:
    	class Result ** results;

		double min_score;
		int32_t threshold;

		uint32_t Capacity;
		uint32_t Size;
		uint32_t alloc;

	public:
		MaxHeap(int, int);
		~MaxHeap();
		int32_t is_empty();
		int32_t is_full();
		void insert(class Result *);
		class Result *remove_head();
};

#endif // ICDS_MaxHeap_H
