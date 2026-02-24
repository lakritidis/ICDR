/**
ICDR: Indexed Contrastive Data Retriever

InvertedList header file: A sequence of compressed (docID, docScore) pairs of integers. It
describes the occurrences of words in documents. The postings are stored in a compressed form.

L. Akritidis, 2026
*/

#ifndef ICDR_INVERTEDLIST_H
#define ICDR_INVERTEDLIST_H

class InvertedList {
	protected:
		struct list_block {
			uint32_t block_size;		/// The size of the block (in postings)
			uint32_t last;				/// The last (greatest) DocID in the block
			uint32_t dwrd;				/// The starting position of the compressed docIDs
			uint32_t swrd;				/// The starting position of the compressed scores
			score_t blockMax_score;		/// Max BM25 score for this block = Max quantized score * scaler.
			score_t scaler;				/// Scaler to convert the float BM25 scores to integers.
		} * skip_table;

		uint32_t * docIDs;				/// Pointer pointing at the compressed docIDs
		uint32_t * dscors;				/// Pointer pointing at the compressed scores

		uint32_t num_postings;			/// The number of postings in the inverted list
		uint32_t dwrd;					/// Length of the compressed docIDs in the list
		uint32_t swrd;					/// Length of the compressed scores in the list
		score_t listMax_score;			/// The maximum IR score among the postings in the list

	private:
		void compress_short_list(score_t, class Records *);
		void compress_long_list(uint32_t, score_t, class Records *);
		void decompress_short_list();
		void decompress_long_list(uint32_t);

		score_t compute_score_scaler(uint32_t, score_t *);
		static int cmp_scores(const void *, const void *);

	public:
		InvertedList();
		InvertedList(uint32_t);
		~InvertedList();

		uint32_t insert_posting(uint32_t);

		/// Methods for compression/decompression and I/O
		void compress(uint32_t, score_t, class Records *);
		void decompress(uint32_t);
		void write(FILE *, uint32_t);
		void read(FILE *);
		void display();
		void display_skip_table(uint32_t);

		/// Mutators
		void set_num_postings(uint32_t);

		/// Accessors
		uint32_t get_num_postings();
		uint32_t get_num_alloc_postings();
		uint32_t get_num_blocks(uint32_t);
		uint32_t * get_docIDs();
		uint32_t * get_dscors();
		uint32_t get_dwrd();
		uint32_t get_swrd();
		uint32_t get_list_block_size();
		score_t get_listMax_score();

		uint32_t get_footprint(uint32_t);

	friend class
		InvertedListIterator;
};

#endif // INVERTEDLIST_H
