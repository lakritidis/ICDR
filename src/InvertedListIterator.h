/**
ICDR: Indexed Contrastive Data Retriever

InvertedListIterator header file: An object that iterates through the compressed postings of an
InvertedList. The object maintains several cursors and two buffers to store the decompressed
docIDs and document scores. The postings are decompressed here during query processing.

L. Akritidis, 2026
*/

#ifndef ICDR_INVERTEDLISTITERATOR_H
#define ICDR_INVERTEDLISTITERATOR_H


class InvertedListIterator {
	private:
		class InvertedList *inv_list;
		uint32_t *dec_docIDs;		/// Pointer pointing at the decompressed docIDs
		uint32_t *dec_dscors;		/// Pointer pointing at the decompressed scores

		/// Temporary data and pointers useful for query processing
		uint32_t *cur_docID;		/// Pointer pointing to a specific docID (posting) value
		uint32_t final_docID;		/// The final docID value of the inverted list
		uint32_t cur_block;			/// The current block (the block that we are now processing)
		uint32_t cur_offset;		/// The offset of the posting within the current block
		bool block_decoded_freq;	/// A flag showing whether the sfreqs of this block have been decoded
		uint32_t block_size;
		uint32_t num_blocks;

	public:
		InvertedListIterator(class InvertedList *, uint32_t);
		~InvertedListIterator();

		void clear();

		bool is_exhausted();
		uint32_t get_cur_docID();
		score_t get_cur_score();
		uint32_t get_final_docID();
		uint32_t search_skip_table (uint32_t, uint32_t, uint32_t);
		void forward_seek(uint32_t, bool);
		bool next();
		void next_shallow(uint32_t);
		void loc_pos_data(int32_t, class result*);
		score_t eval_posting_BM25(score_t, score_t);
		void assign_short_list_data();

		/// Decompression Functions
		uint32_t * decode_docIDs(uint32_t);
		uint32_t * decode_scores(uint32_t);

		uint32_t get_freq();
		uint32_t get_cur_block();
		uint32_t get_cur_offset();
		uint32_t get_num_blocks();
		uint32_t get_num_postings();

		score_t get_cur_blockMax_score();
		score_t get_listMax_score();

		void display_skip_table(uint32_t);
};

#endif // INVERTEDLISTITERATOR_H
