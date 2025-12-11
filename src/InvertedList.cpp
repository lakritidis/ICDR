/// ICDR: Indexed Contrastive Data Retriever

/// InvertedList Implementation File: A sequence of (docID, frequency) pairs of integers. It
/// describes the occurrences of words in documents. The postings are decompressed here.
///
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_INVERTEDLIST_CPP
#define ICDS_INVERTEDLIST_CPP

#include "InvertedList.h"

/// Inverted List default constructor
InvertedList::InvertedList() :
	skip_table(NULL),
	num_postings(0),
	num_alloc_postings(0),
	dwrd(0),
	fwrd(0),
	docIDs(NULL),
	dfreqs(NULL) {
}

/// Inverted List constructor with initial buffers for docIDs and Frequencies
InvertedList::InvertedList(uint32_t alloc) :
	skip_table(NULL),
	num_postings(0),
	num_alloc_postings(alloc),
	dwrd(0),
	fwrd(0),
	docIDs(NULL),
	dfreqs(NULL) {

		this->docIDs = (uint32_t *)malloc(this->num_alloc_postings * sizeof(uint32_t));
		this->dfreqs = (uint32_t *)malloc(this->num_alloc_postings * sizeof(uint32_t));
}

/// Inverted List destructor
InvertedList::~InvertedList() {
	if (this->skip_table) {
		free(this->skip_table);
	}

	if (this->docIDs) {
		free(this->docIDs);
	}

	if (this->dfreqs) {
		free(this->dfreqs);
	}
}

/// Insert a posting (i.e. term occurrence) in the inverted list.
uint32_t InvertedList::insert_posting(uint32_t d) {
	uint32_t result = 0;

	if (this->num_postings == 0) {
		this->docIDs[this->num_postings] = d;
		this->dfreqs[this->num_postings] = 1;
		this->num_postings++;
		result = 1;
	} else {
		if (this->docIDs[this->num_postings - 1] == d) {
			this->dfreqs[this->num_postings - 1] += 1;
			result = 2;
		} else {
			this->docIDs[this->num_postings] = d;
			this->dfreqs[this->num_postings] = 1;
			this->num_postings++;
			result = 1;
		}
	}

	if (this->num_postings >= this->num_alloc_postings) {
		this->num_alloc_postings *= 2;
		this->docIDs = (uint32_t *)realloc(this->docIDs, this->num_alloc_postings * sizeof(uint32_t));
		this->dfreqs = (uint32_t *)realloc(this->dfreqs, this->num_alloc_postings * sizeof(uint32_t));
	}
	return result;
}

/// ///////////////////////////////////////////////////////////////////////////////////////////////
/// Inverted List compression/decompression methods

/// Compress a short Inverted list (fewer than 129 postings, 1 block) with Variable Byte.
void InvertedList::compress_short_list() {
	uint32_t previous_d = 0, d_gap = 0;
	uint32_t * temp_ptr, * in_ptr, * out_ptr;

	// printf("Short List with %d postings and %d blocks\n", this->num_postings, this->get_num_blocks(128));
	// this->display();

	this->skip_table = NULL;

	/// Replace the document IDs boy document ID gaps
	for (uint32_t i = 0; i < this->num_postings; i++) {
		d_gap = this->docIDs[i] - previous_d;
		previous_d = this->docIDs[i];
		this->docIDs[i] = d_gap;
	}

	/// Allocate space to store the compressed data
	uint32_t * encoded_docIDs = (uint32_t *)calloc(this->num_postings, sizeof(uint32_t));
	uint32_t * encoded_dfreqs = (uint32_t *)calloc(this->num_postings, sizeof(uint32_t));

	/// Compress the document ID gaps - shrink the allocated space for the compressed docID gaps.
	/// Update the Inverted List docID pointer to "look" into the compressed docID gaps.
	in_ptr = this->docIDs;
	out_ptr = encoded_docIDs;
	this->dwrd = VBYTE_CODER->Compression(in_ptr, out_ptr, this->num_postings);
	encoded_docIDs = (uint32_t *)realloc(encoded_docIDs, this->dwrd * sizeof(uint32_t));
	temp_ptr = this->docIDs;
	this->docIDs = encoded_docIDs;
	free(temp_ptr);

	/// Compress the document frequencies - shrink the allocated space for the compressed frequencies.
	/// Update the Inverted List dfreq pointer to "look" into the compressed document frequencies.
	in_ptr = this->dfreqs;
	out_ptr = encoded_dfreqs;
	this->fwrd = VBYTE_CODER->Compression(in_ptr, out_ptr, this->num_postings);
	encoded_dfreqs = (uint32_t *)realloc(encoded_dfreqs, this->fwrd * sizeof(uint32_t));
	temp_ptr = this->dfreqs;
	this->dfreqs = encoded_dfreqs;
	free(temp_ptr);
}

/// Compress a long Inverted list (more than 128 postings, more than 1 block) with PForDelta.
void InvertedList::compress_long_list(uint32_t block_size) {
	uint32_t d_gap = 0, previous_d = 0, i = 0, x = 0, adv = 0;
	uint32_t num_blocks = this->get_num_blocks(block_size), cur_block = 0;
	uint32_t * temp_ptr, * in_ptr, * out_ptr;

	/// Allocate space to store the compressed data
	uint32_t * encoded_docIDs = (uint32_t*)calloc(num_blocks * this->num_postings, sizeof(uint32_t));
	uint32_t * encoded_dfreqs = (uint32_t*)calloc(num_blocks * this->num_postings, sizeof(uint32_t));

	/// Expand the docID and frequency buffers so that they are integrals of block_size
	this->docIDs = (uint32_t *)realloc(this->docIDs, num_blocks * block_size * sizeof(uint32_t));
	this->dfreqs = (uint32_t *)realloc(this->dfreqs, num_blocks * block_size * sizeof(uint32_t));

	/// This is a long list: Initialize the skip table
	this->skip_table = (skip_structure *)malloc(num_blocks * sizeof(skip_structure));
	for (uint32_t i = 0; i < num_blocks; i++) {
		this->skip_table[i].block_size = 0;
		this->skip_table[i].dwrd = 0;
		this->skip_table[i].fwrd = 0;
		this->skip_table[i].last = 0;
		//this->skip_table[i].max_score = 1;
	}

	for (i = 0; i < this->num_postings; i++) {
		/// Construct a new block: We check whether a block of docIds is complete.
		/// If it is, use a block of BLOCK_SIZE size, and encode it by using PFOR-DELTA.
		if (x >= block_size) {
			// printf("Block %d - Start Posting: %d, End Posting: %d\n", cur_block + 1, adv, i);

			/// Update the skip table
			this->skip_table[cur_block].block_size = block_size;
			this->skip_table[cur_block].dwrd = this->dwrd;
			this->skip_table[cur_block].fwrd = this->fwrd;
			this->skip_table[cur_block].last = previous_d;
			//this->skip_table[cur_block].max_score = 1;

			/// Compress the docID gaps
			in_ptr = this->docIDs + adv;
			out_ptr = encoded_docIDs + this->dwrd;
			this->dwrd += PFOR_CODER->Compression(in_ptr, out_ptr, this->num_postings);

			/// Compress the document frequency block
			in_ptr = this->dfreqs + adv;
			out_ptr = encoded_dfreqs + this->fwrd;
			this->fwrd += PFOR_CODER->Compression(in_ptr, out_ptr, this->num_postings);

			adv += block_size;
			cur_block += 1;
			x = 0;
		}

		d_gap = this->docIDs[i] - previous_d;
		previous_d = this->docIDs[i];
		this->docIDs[i] = d_gap;
		x++;
	}

	/// Construct the final block of the list
	// printf("Block %d (final) - Start Posting: %d, End Posting: %d\n", cur_block + 1, adv, i);
	/// Pad the last block with zeroes
	for (i = this->num_postings; i < num_blocks * block_size; i++) {
		this->docIDs[i] = 0;
		this->dfreqs[i] = 0;
	}

	/// Update the skip table
	this->skip_table[cur_block].block_size = x;
	this->skip_table[cur_block].dwrd = this->dwrd;
	this->skip_table[cur_block].fwrd = this->fwrd;
	this->skip_table[cur_block].last = previous_d;
	//this->skip_table[cur_block].max_score = 1;

	/// Compress the docID gaps
	in_ptr = this->docIDs + adv;
	out_ptr = encoded_docIDs + this->dwrd;
	this->dwrd += PFOR_CODER->Compression(in_ptr, out_ptr, this->num_postings);
	encoded_docIDs = (uint32_t *)realloc(encoded_docIDs, this->dwrd * sizeof(uint32_t));
	temp_ptr = this->docIDs;
	this->docIDs = encoded_docIDs;
	free(temp_ptr);

	/// Compress the document frequency block
	in_ptr = this->dfreqs + adv;
	out_ptr = encoded_dfreqs + this->fwrd;
	this->fwrd += PFOR_CODER->Compression(in_ptr, out_ptr, this->num_postings);
	encoded_dfreqs = (uint32_t *)realloc(encoded_dfreqs, this->fwrd * sizeof(uint32_t));
	temp_ptr = this->dfreqs;
	this->dfreqs = encoded_dfreqs;
	free(temp_ptr);
}

/// Compress an Inverted List by creating and attaching an InvertedListHandler object
void InvertedList::compress(uint32_t block_size) {
	/// Handle the short lists
	if (this->get_num_blocks(block_size) == 1) {
		this->compress_short_list();

	/// Handle the long lists (more than block_size postings)
	} else {
		this->compress_long_list(block_size);
	}
}

/// Decompress a short Inverted list (fewer than 129 postings, 1 block) with Variable Byte.
void InvertedList::decompress_short_list() {
	uint32_t prev_d = 0;
	uint32_t * in_ptr, * out_ptr;

	printf("\tDecompressing short Inverted List with %d postings\n", this->num_postings);

	/// Allocate space to store the decompressed data
	uint32_t * decoded_docIDs = (uint32_t*)calloc(this->num_postings, sizeof(uint32_t));
	uint32_t * decoded_dfreqs = (uint32_t*)calloc(this->num_postings, sizeof(uint32_t));

	/// Decompress the document ID gaps
	/// Update the Inverted List docID pointer to "look" into the decompressed docID gaps.
	in_ptr = this->docIDs;
	out_ptr = decoded_docIDs;
	VBYTE_CODER->Decompression(in_ptr, out_ptr, this->num_postings);
	free(this->docIDs);
	/// Reconstruct the original document IDs from docID gaps.
	for (uint32_t i = 0; i < this->num_postings; i++) {
		out_ptr[i] += prev_d;
		prev_d = out_ptr[i];
	}
	this->docIDs = decoded_docIDs;

	/// Decompress the document frequencies
	/// Update the Inverted List dfreq pointer to "look" into the decompressed document frequencies.
	in_ptr = this->dfreqs;
	out_ptr = decoded_dfreqs;
	VBYTE_CODER->Decompression(in_ptr, out_ptr, this->num_postings);
	free(this->dfreqs);
	this->dfreqs = decoded_dfreqs;
}

/// Decompress a long Inverted list (more than 128 postings, more than 1 block) with PForDelta.
void InvertedList::decompress_long_list(uint32_t block_size) {
	uint32_t i = 0, j = 0, prev_d = 0;
	uint32_t * in_ptr, * out_ptr;
	uint32_t num_blocks = this->get_num_blocks(block_size);

	printf("\tInverted List Organization:\n\t\tPostings = %d, Blocks = %d, Block Size = %d\n",
		this->num_postings, num_blocks, block_size);

	uint32_t * decoded_docIDs = (uint32_t *)calloc(num_blocks * block_size, sizeof(uint32_t));
	uint32_t * decoded_dfreqs = (uint32_t *)calloc(num_blocks * block_size, sizeof(uint32_t));

	/// Decompress the document ID gaps
	in_ptr = this->docIDs;
	out_ptr = decoded_docIDs;
	for (i = 0; i < num_blocks; i++) {
		in_ptr = this->docIDs + this->skip_table[i].dwrd;
		out_ptr = decoded_docIDs + i * block_size;
		PFOR_CODER->Decompression(in_ptr, out_ptr, block_size);

		/// Reconstruct the original document IDs from docID gaps.
		if (i > 0) { prev_d = this->skip_table[i - 1].last; }
		for (j = 0; j < block_size; j++) {
			out_ptr[j] += prev_d;
			prev_d = out_ptr[j];
		}
	}

	/// Decompress the document frequencies
	for (i = 0; i < num_blocks; i++) {
		in_ptr = this->dfreqs + this->skip_table[i].fwrd;
		out_ptr = decoded_dfreqs + i * block_size;
		PFOR_CODER->Decompression(in_ptr, out_ptr, block_size);
	}

	/// Update the Inverted List docID pointer to "look" into the decompressed docID gaps.
	free(this->docIDs);
	this->docIDs = decoded_docIDs;

	/// Update the Inverted List dfreq pointer to "look" into the decompressed document frequencies.
	free(this->dfreqs);
	this->dfreqs = decoded_dfreqs;
}

/// Decompress the entire inverted list. The compressed data is stored in this->docIDs and
/// this->dfreq. After decompression, this->docIDs and this->dfreq point to the decoded data.
void InvertedList::decompress(uint32_t block_size) {
	/// Handle the short lists
	if (this->get_num_blocks(block_size) == 1) {
		this->decompress_short_list();

	/// Handle the long lists (more than block_size postings)
	} else {
		this->decompress_long_list(block_size);
	}
}

/// //////////////////////////////////////////////////////////////////////////////////////////////
/// Inverted List IO
/// Write an inverted list to disk
void InvertedList::write(FILE * fp, uint32_t block_size) {
	uint32_t num_blocks = this->get_num_blocks(block_size);

	fwrite(&this->num_postings, sizeof(uint32_t), 1, fp);
	fwrite(&num_blocks, sizeof(uint32_t), 1, fp);
	fwrite(&this->dwrd, sizeof(uint32_t), 1, fp);
	fwrite(&this->fwrd, sizeof(uint32_t), 1, fp);

	/// If this is a long list, write the skip table in a compressed form (with VByte)
	if (num_blocks > 1) {
		/// Create a BitVictor that will store the compressed skip table
		class BitVector<uint32_t> *skiptable_vector = new BitVector<uint32_t>(100);

		for (uint32_t i = 0; i < num_blocks; i++) {
			// printf("Block: %d, size: %d, last: %d, dwrd: %d, fwrd: %d\n",
			//	i + 1, this->skip_table[i].block_size, this->skip_table[i].last,
			//	this->skip_table[i].dwrd, this->skip_table[i].fwrd);

			skiptable_vector->VByte_Encode(this->skip_table[i].block_size);
			skiptable_vector->VByte_Encode(this->skip_table[i].last);
			skiptable_vector->VByte_Encode(this->skip_table[i].dwrd);
			skiptable_vector->VByte_Encode(this->skip_table[i].fwrd);
			//skiptable_vector->VByte_Encode(this->skip_table[i].max_score);
		}

		skiptable_vector->pad_vec();
		skiptable_vector->dump_vec(fp);
		skiptable_vector->free_vec();
	}

	/// Write the compressed docID gaps and document frequencies.
	fwrite(this->docIDs, sizeof(uint32_t), this->dwrd, fp);
	fwrite(this->dfreqs, sizeof(uint32_t), this->fwrd, fp);
}

/// Read an inverted list from disk
void InvertedList::read(FILE * fp) {
	uint32_t num_blocks = 0;

	fread(&this->num_postings, sizeof(uint32_t), 1, fp);
	fread(&num_blocks, sizeof(uint32_t), 1, fp);
	fread(&this->dwrd, sizeof(uint32_t), 1, fp);
	fread(&this->fwrd, sizeof(uint32_t), 1, fp);

	this->skip_table = NULL;

	/// If this is a long list, read the skip table in a compressed form (with VByte)
	if (num_blocks > 1) {
		/// Read the compressed skip table
		class BitVector<uint32_t> *skiptable_vector = new BitVector<uint32_t>();
		skiptable_vector->read_vec(fp);

		this->skip_table = (skip_structure *)malloc(num_blocks * sizeof(skip_structure));
		for (uint32_t i = 0; i < num_blocks; i++) {
			this->skip_table[i].block_size = skiptable_vector->VByte_Decode();
			this->skip_table[i].last = skiptable_vector->VByte_Decode();
			this->skip_table[i].dwrd = skiptable_vector->VByte_Decode();
			this->skip_table[i].fwrd = skiptable_vector->VByte_Decode();
			//this->skip_table[i].max_score = skiptable_vector->VByte_Decode();

			// printf("Block: %d, size: %d, last: %d, dwrd: %d, fwrd: %d\n",
			//	i + 1, this->skip_table[i].block_size, this->skip_table[i].last,
			//	this->skip_table[i].dwrd, this->skip_table[i].fwrd);
		}
		skiptable_vector->free_vec();
	}

	this->docIDs = (uint32_t *)calloc(this->dwrd, sizeof(uint32_t));
	this->dfreqs = (uint32_t *)calloc(this->fwrd, sizeof(uint32_t));

	/// Read the compressed docID gaps and document frequencies.
	fread(this->docIDs, sizeof(uint32_t), this->dwrd, fp);
	fread(this->dfreqs, sizeof(uint32_t), this->fwrd, fp);
}

/// Display the inverted list contents - Decompression takes place on the fly
void InvertedList::display() {
	this->decompress(128);

	printf("[ ");
	for (uint32_t i = 0; i < this->num_postings; i++) {
		printf("P-%d: (%d, %d) ", i + 1, this->docIDs[i], this->dfreqs[i]);
	}
	printf("]");
	fflush(NULL);
}

/// Accessors
inline uint32_t * InvertedList::get_docIDs() { return this->docIDs; }
inline uint32_t * InvertedList::get_dfreqs() { return this->dfreqs; }
inline uint32_t InvertedList::get_num_alloc_postings() { return this->num_alloc_postings; }
inline uint32_t InvertedList::get_num_postings() { return this->num_postings; }
uint32_t InvertedList::get_num_blocks(uint32_t block_size) {
	if (this->num_postings % block_size == 0) {
		return this->num_postings / block_size;
	} else {
		return this->num_postings / block_size + 1;
	}
}

/// Mutators
void InvertedList::set_num_postings(uint32_t v) { this->num_postings = v; }

#endif
