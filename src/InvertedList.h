/// ICDR: Indexed Contrastive Data Retriever

/// InvertedList Implementation File: A sequence of (docID, frequency) pairs of integers. It
/// describes the occurrences of words in documents. The postings are decompressed here.
///
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef INVERTEDLIST_H
#define INVERTEDLIST_H

class InvertedList {
	protected:
		struct skip_structure {
			uint32_t block_size;	/// The size of the block (in postings)
			uint32_t last;			/// The last Document ID of a block of the inverted list
			uint32_t dwrd;			/// The starting position of the compressed document IDs
			uint32_t fwrd;			/// The starting position of the compressed frequencies
			//uint32_t max_score;		/// For Block-MAX WAND
		} * skip_table;

		uint32_t num_postings;
		uint32_t num_alloc_postings;

		uint32_t dwrd;			/// The overall length of the compressed document IDs in the list
		uint32_t fwrd;			/// The overall length of the compressed frequencies in the list

		uint32_t * docIDs;		/// Pointer pointing at the compressed docIDs
		uint32_t * dfreqs;		/// Pointer pointing at the compressed frequencies

	private:
		void compress_short_list();
		void compress_long_list(uint32_t);
		void decompress_short_list();
		void decompress_long_list(uint32_t);

	public:
		InvertedList();
		InvertedList(uint32_t);
		~InvertedList();

		uint32_t insert_posting(uint32_t);

		/// Methods for compression/decompression and I/O
		void compress(uint32_t);
		void decompress(uint32_t);
		void write(FILE *, uint32_t);
		void read(FILE *);
		void display();

		/// Accessors/Mutators
		uint32_t get_num_postings();
		uint32_t get_num_alloc_postings();
		uint32_t get_num_blocks(uint32_t);
		uint32_t * get_docIDs();
		uint32_t * get_dfreqs();

		void set_handler(class InvertedListHandler *);
		void set_num_postings(uint32_t);

	friend class
		InvertedListIterator;
};

#endif // INVERTEDLIST_H
