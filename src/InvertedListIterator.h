/// ICDR: Indexed Contrastive Data Retriever

/// InvertedListIterator Header File: An object that iterates through pairs of integers. It
/// describes the occurrences of words in documents. The postings are decompressed here.
///
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef INVERTEDLISTITERATOR_H
#define INVERTEDLISTITERATOR_H


class InvertedListIterator {
	private:
		class InvertedList *inv_list;
		uint32_t *dec_docIDs;		/// Pointer pointing at the decompressed docIDs
		uint32_t *dec_dfreqs;		/// Pointer pointing at the decompressed frequencies

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
		uint32_t search_skip_table (uint32_t, uint32_t, uint32_t);
		void forward_seek(uint32_t);
		bool next();
		void loc_pos_data(int32_t, class result*);
		score_t eval_posting_BM25(score_t, score_t, score_t);
		void assign_short_list_data();

		/// Decompression Functions
		uint32_t decode_var_byte(class BitVector<uint32_t> *, uint32_t, uint32_t);
		uint32_t *decode_docIDs(uint32_t);
		uint32_t *decode_frequencies(uint32_t);

		uint32_t get_freq();
};

#endif // INVERTEDLISTITERATOR_H
