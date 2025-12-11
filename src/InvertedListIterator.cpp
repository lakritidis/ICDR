/// ICDR: Indexed Contrastive Data Retriever

/// InvertedListIterator Implementation File: An object that iterates through pairs of integers. It
/// describes the occurrences of words in documents. The postings are decompressed here.
///
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_INVERTEDLISTITERATOR_CPP
#define ICDS_INVERTEDLISTITERATOR_CPP

#include "InvertedListIterator.h"

/// InvertedListIterator constructor - an inverted list is required to create an iterator
InvertedListIterator::InvertedListIterator(class InvertedList * il, uint32_t block_size) :
	inv_list( il ),
	dec_docIDs( new uint32_t[block_size * sizeof(uint32_t)]() ),
	dec_dfreqs( new uint32_t[block_size * sizeof(uint32_t)]() ),
	cur_docID(NULL),
	final_docID(0),
	cur_block(0),
	cur_offset(0),
	block_decoded_freq(false),
	block_size( block_size ),
	num_blocks( il->get_num_blocks(block_size) ) {
		if (num_blocks > 1) {
			this->final_docID = il->skip_table[num_blocks - 1].last;
		}
}

/// Destructor
InvertedListIterator::~InvertedListIterator() {
	if (this->dec_docIDs) { delete [] this->dec_docIDs; }
	if (this->dec_dfreqs) { delete [] this->dec_dfreqs; }
}

/// Evaluate a posting of the Inverted List according to BM25
score_t InvertedListIterator::eval_posting_BM25(score_t K, score_t idf, score_t k1) {

	score_t score = 0.0;
	uint32_t tfreq = 0;

	/// Decode the frequecy chunk (in case it is not already decoded) and if it is a long list.
	if (!this->block_decoded_freq) {
		this->block_decoded_freq = true;
		this->decode_frequencies(this->cur_block);
	}

	/// Find the term frequency within that posting by dereferencing dec_freqs[this->cur_offset]
	tfreq = this->dec_dfreqs[this->cur_offset];

	/// Compute the BM25 Score
	score = (idf * tfreq * (k1 + 1)) / ( tfreq + K );

	return score;
}

/// Decode the DocIDs of a given block
uint32_t * InvertedListIterator::decode_docIDs(uint32_t block) {
	uint32_t y = 0;
	uint32_t *din_ptr, *dout_ptr, prev_d = 0;

	/// Clear the buffer storing the doc IDs
	memset(this->dec_docIDs, 0, this->block_size * sizeof(uint32_t));

	/// Decode a docID block that belongs to a short list: No skip table exists and we use VBYTE
	if (this->num_blocks == 1) {
		uint32_t cur_block_size = this->inv_list->get_num_postings();

		din_ptr = this->inv_list->docIDs;
		dout_ptr = this->dec_docIDs;

		/// Decompress
		VBYTE_CODER->Decompression(din_ptr, dout_ptr, cur_block_size);

		for (y = 0; y < cur_block_size; y++) {
			this->dec_docIDs[y] += prev_d;
			prev_d = this->dec_docIDs[y];
			// printf("DocID %d: %d\n", y + 1, this->dec_docIDs[y]); fflush(NULL);
		}
		this->final_docID = this->dec_docIDs[y - 1];

	/// Decode a docID block that belongs to a long list: Refer to the skip table exists and use P4D
	} else {
		PFOR_CODER->set_size(this->block_size);

		din_ptr = this->inv_list->docIDs + this->inv_list->skip_table[block].dwrd;
		dout_ptr = this->dec_docIDs;

		/// Decompress
		PFOR_CODER->Decompression(din_ptr, dout_ptr, this->block_size);

		/// Reconstruct the original DocIDs by summing the d_gaps
		if (block == 0) {
			prev_d = 0;
		} else {
			prev_d = this->inv_list->skip_table[block - 1].last;
		}

		for (y = 0; y < this->block_size; y++) {
			this->dec_docIDs[y] += prev_d;
			prev_d = this->dec_docIDs[y];
			// printf("DocID %d: %d\n", y + 1, this->dec_docIDs[y]);
		}
	}

	this->cur_docID = dout_ptr;
	this->cur_offset = 0;

	return dout_ptr;
}


/// Decode the frequencies of a specific block
uint32_t *InvertedListIterator::decode_frequencies(uint32_t block) {

	uint32_t *fin_ptr, *fout_ptr;
	uint32_t cur_block_size;

	/// Clear the buffer storing the doc IDs
	memset(this->dec_dfreqs, 0, this->block_size * sizeof(uint32_t));

	/// Decode a dfreq block that belongs to a short list: No skip table exists and we use VBYTE
	if (this->num_blocks == 1) {
		cur_block_size = this->inv_list->get_num_postings();

		fin_ptr = this->inv_list->dfreqs;
		fout_ptr = this->dec_dfreqs;

		/// Decompress
		VBYTE_CODER->Decompression(fin_ptr, fout_ptr, cur_block_size);
/*
		for (uint32_t y = 0; y < cur_block_size; y++) {
			printf("DocFreq %d: %d\n", y + 1, this->dec_dfreqs[y]);
		}
*/
	/// Decode a dfreq block that belongs to a long list: Refer to the skip table exists and use P4D
	} else {
		cur_block_size = this->inv_list->skip_table[block].block_size;
		PFOR_CODER->set_size(cur_block_size);

		fin_ptr = this->inv_list->dfreqs + this->inv_list->skip_table[block].fwrd;
		fout_ptr = this->dec_dfreqs;

		/// Decompress
		PFOR_CODER->Decompression(fin_ptr, fout_ptr, this->block_size);
	}
	this->block_decoded_freq = true;

	return fout_ptr;
}

/// Seek forward in the inverted list to locate and decode the block containing the docID we need.
void InvertedListIterator::forward_seek(uint32_t search_docId) {

	int32_t res = 0;
	uint32_t blk_size = this->block_size;
	if (this->inv_list->skip_table) {
		blk_size = this->inv_list->skip_table[this->cur_block].block_size;
	}

	/// Keep searching until search_docId is reached
	while (*(this->cur_docID) < search_docId) {

		/// In case we have reached the end of the inverted list, exit
		if (*(this->cur_docID) == this->final_docID) {
			return;
		} else {
			this->cur_docID++;
			this->cur_offset++;
		}

		/// In case of a short list, we check that have reached the end without accessing the skip
		/// structure (remember: for short lists, we do not allocate space for a skip table)
		if (this->inv_list->num_postings < this->block_size) {
			if (this->cur_docID == this->dec_docIDs + this->inv_list->num_postings - 1) {
				return;
			}
		} else {

			/// In case the list is long we check whether we have reached at the end of the current
			/// buffer. In this case we search the associated skip table to determine the next
			/// block to decompress.
			if (this->cur_docID == this->dec_docIDs + blk_size) {

				/// Search
				res = this->search_skip_table(this->cur_block + 1,
					this->inv_list->get_num_blocks(this->block_size) - 1, search_docId);

				blk_size = this->inv_list->skip_table[this->cur_block].block_size;

				/// If the search is not successful, break the operation
				if (res == 0) {
					break;
				}

				// printf("\n\t\tEnd of block reached while searching for docID %d - ", search_docId);
				// printf("Decompressing block %d...\n", this->cur_block);
				this->decode_docIDs(this->cur_block);
			}
		}
	}
	// printf("Reached %d\n", (*this->cur_docID));
}

/// Progress the pointer to the next posting. In case we reach the end of a block, decompress the
/// next one. If there is no next block, signify the end of the inverted list by returning false.
bool InvertedListIterator::next() {
	uint32_t num_postings = this->inv_list->get_num_postings();

	if (num_postings < this->block_size) {
		if (this->cur_docID == this->dec_docIDs + num_postings - 1) {
			return false;
		} else {
			this->cur_docID++;
			this->cur_offset++;
			return true;
		}
	}

	/// Progress the inverted list pointer to the next posting
	this->cur_docID++;
	this->cur_offset++;

	/// In case we have reached the end of the inverted list block, decode the next one
	if (this->cur_docID == this->dec_docIDs + this->inv_list->skip_table[this->cur_block].block_size) {
//		printf("Block: %d/%d\n", this->cur_block, this->num_blocks); fflush(NULL);
		/// If we have not reached the last block.
		if (this->cur_block < this->inv_list->get_num_blocks(this->block_size) - 1) {
			decode_docIDs(this->cur_block + 1);

			this->cur_block++;
			this->block_decoded_freq = false;
		} else {
			return false;
		}
	}

	return true;
}

/// Given a document ID, perform a binary search on the skip table to locate the respective block.
uint32_t InvertedListIterator::search_skip_table(uint32_t first, uint32_t last, uint32_t key) {
/*
	printf ("Searching for DocID %d...\n", key);
	printf("\n");
	if (this->inv_list->skip_table) {
		for (uint32_t i = 0; i < this->inv_list->get_num_blocks(this->block_size); i++) {
			printf("Block %d max docid = %d\n", i, this->inv_list->skip_table[i].last);
		}
	}
*/
	uint32_t mid = 0;
	while (first <= last) {
		mid = (first + last) / 2;

		if (key > this->inv_list->skip_table[mid].last) {
			first = mid + 1;
		} else if (key < this->inv_list->skip_table[mid].last) {
			last = mid - 1;
		} else {
			this->cur_block = mid;
			this->block_decoded_freq = false;
			return mid;
		}
	}

	if (this->inv_list->skip_table[mid].last < key) { mid++; }

	// printf("Exiting Next Block: %d. Last DocID: %d. ", mid, this->inv_list->skip_table[mid].last); getchar();
	this->cur_block = mid;
	this->block_decoded_freq = false;
	return mid;

}

/// This function checks whether we have reached the end of the inverted list
inline bool InvertedListIterator::is_exhausted() {
	if (*(this->cur_docID) >= this->final_docID) {
		// printf("Exhausted -- Cur. docID: %d, Last DocID: %d\n", *(this->cur_docID), this->final_docID);
		return true;
	}
	return false;
}


inline void InvertedListIterator::clear() {
	memset(this->dec_docIDs, 0, this->block_size * sizeof(uint32_t));
	memset(this->dec_dfreqs, 0, this->block_size * sizeof(uint32_t));

	this->cur_docID = NULL;
	this->final_docID = 0;
	this->cur_block = 0;
	this->cur_offset = 0;
	this->block_decoded_freq = false;

//	this->inv_list->clear();
}

inline uint32_t InvertedListIterator::get_cur_docID() { return *(this->cur_docID); }
inline uint32_t InvertedListIterator::get_freq() { return this->inv_list->get_num_postings(); }
#endif
