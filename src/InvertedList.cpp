/**
ICDR: Indexed Contrastive Data Retriever

InvertedList implementation file: A sequence of compressed (docID, docScore) pairs of integers. It
describes the occurrences of words in documents. The postings are stored in a compressed form.

L. Akritidis, 2026
*/

#ifndef ICDR_INVERTEDLIST_CPP
#define ICDR_INVERTEDLIST_CPP

#include "InvertedList.h"

/// Inverted List default constructor. Initializes a completely empty list.
InvertedList::InvertedList() :
	skip_table(NULL),
	docIDs(NULL),
	dscors(NULL),
	num_postings(0),
	dwrd(0),
	swrd(0),
	listMax_score(0) {
}

/// Inverted List constructor with initial buffers for the compressed docIDs and BM25 Scores.
InvertedList::InvertedList(uint32_t alloc) :
	skip_table(NULL),
	docIDs(NULL),
	dscors(NULL),
	num_postings(0),
	dwrd(alloc),
	swrd(alloc),
	listMax_score(0) {

		this->docIDs = (uint32_t *)malloc(this->dwrd * sizeof(uint32_t));
		this->dscors = (uint32_t *)malloc(this->swrd * sizeof(uint32_t));
}

/// Inverted List destructor
InvertedList::~InvertedList() {
	if (this->skip_table) {
		free(this->skip_table);
	}

	if (this->docIDs) {
		free(this->docIDs);
	}

	if (this->dscors) {
		free(this->dscors);
	}
}

/// Insert a posting (i.e. term occurrence) in the inverted list. Initially, we store the term
/// frequency in the posting. It will be later converted to quantized BM25 scores, after several
/// necessary global statistics (e.g. idfs, average doc length) have been computed.
uint32_t InvertedList::insert_posting(uint32_t d) {
	uint32_t result = 0;

	if (this->num_postings == 0) {
		this->docIDs[this->num_postings] = d;
		this->dscors[this->num_postings] = 1;
		this->num_postings++;
		result = 1;
	} else {
		if (this->docIDs[this->num_postings - 1] == d) {
			this->dscors[this->num_postings - 1] += 1;
			result = 2;
		} else {
			this->docIDs[this->num_postings] = d;
			this->dscors[this->num_postings] = 1;
			this->num_postings++;
			result = 1;
		}
	}

	if (this->num_postings >= this->dwrd) {
		this->dwrd *= 2;
		this->swrd *= 2;
		this->docIDs = (uint32_t *)realloc(this->docIDs, this->dwrd * sizeof(uint32_t));
		this->dscors = (uint32_t *)realloc(this->dscors, this->swrd * sizeof(uint32_t));
	}
	return result;
}

/// ///////////////////////////////////////////////////////////////////////////////////////////////
/// Inverted List compression/decompression methods

/// Compute the scaler to quantize the float BM25 scores. The scaler converts the minimum (non-zero)
/// score difference -among the block postings- to 1.
score_t InvertedList::compute_score_scaler(uint32_t block_size, score_t *scores) {
	score_t diff = 0.0f, min_diff = 1 << 10;

	qsort(scores, block_size, sizeof(score_t), &InvertedList::cmp_scores);

	for (uint32_t i = 0; i < block_size; i++) {
		if (i < block_size - 1) {
			diff = scores[i + 1] - scores[i];
			// printf("score %d (%5.5f) vs score %d (%5.5f), diff = %5.5f\n", i+1, scores[i+1], i, scores[i], diff);
			if (diff < min_diff && diff > 0.01f) {
				min_diff = diff;
			}
		}
	}
	// printf("Min diff: %5.3f", min_diff);
	// q = round(score / max_score * (2^b - 1));
	return 1.0f / min_diff;
}

/// Compress a short Inverted list (fewer than 129 postings, 1 block) with Variable Byte.
void InvertedList::compress_short_list(score_t idf, class Records * recs) {
	uint32_t previous_d = 0, d_gap = 0, i = 0;
	uint32_t * temp_ptr, * in_ptr, * out_ptr;

	score_t score = 0.0f, scores[this->num_postings], scaler = SHL_SCALER;
	score_t K = 0.0f, dl = 0.0f, avgdl = recs->get_avg_doc_len();

	for (i = 0; i < this->num_postings; i++) {
		dl = (score_t)recs->get_doc_len(this->docIDs[i] - 1);
		K = BM25_k2_PARAM * (1.0f - BM25_b_PARAM + BM25_b_PARAM * dl / avgdl);
		score = idf * this->dscors[i] * (BM25_k1_PARAM + 1.0f) / (this->dscors[i] + K);

		scores[i] = score;
		// printf("\tDocID=%d, DL=%5.1f, AvgDL=%5.3f, K=%5.3f, idf=%5.3f, df=%d, score=%5.3f\n",
		//	this->docIDs[i], dl, avgdl, K, idf, this->dscors[i], score);
	}

	this->skip_table = NULL;

	/// Replace the document IDs by document ID gaps
	for (i = 0; i < this->num_postings; i++) {
		d_gap = this->docIDs[i] - previous_d;
		previous_d = this->docIDs[i];
		this->docIDs[i] = d_gap;
	}

	/// Allocate space to store the compressed data
	uint32_t * encoded_docIDs = (uint32_t *)calloc(this->num_postings, sizeof(uint32_t));
	uint32_t * encoded_dscors = (uint32_t *)calloc(this->num_postings, sizeof(uint32_t));

	/// Compress the document ID gaps - shrink the allocated space for the compressed docID gaps.
	/// Update the Inverted List docID pointer to "look" into the compressed docID gaps.
	in_ptr = this->docIDs;
	out_ptr = encoded_docIDs;
	this->dwrd = VBYTE_CODER->Compression(in_ptr, out_ptr, this->num_postings);
	encoded_docIDs = (uint32_t *)realloc(encoded_docIDs, this->dwrd * sizeof(uint32_t));
	temp_ptr = this->docIDs;
	this->docIDs = encoded_docIDs;
	free(temp_ptr);

	/// Quantize and compress the BM25 scores - shrink the allocated space for the compressed scores.
	/// In short lists we do not have a scaler; we quantize with q=round(score/max_score * (2^b - 1));
	/// Update the Inverted List dscors pointer to "look" into the compressed document scores.
	in_ptr = this->dscors;
	for (i = 0; i < this->num_postings; i++) {
		in_ptr[i] = (uint32_t) ( scaler * scores[i] );
		score = in_ptr[i] / scaler;
		if (score > this->listMax_score) { this->listMax_score = score; }
	}
	out_ptr = encoded_dscors;
	this->swrd = VBYTE_CODER->Compression(in_ptr, out_ptr, this->num_postings);
	encoded_dscors = (uint32_t *)realloc(encoded_dscors, this->swrd * sizeof(uint32_t));
	temp_ptr = this->dscors;
	this->dscors = encoded_dscors;
	free(temp_ptr);
}

/// Compress a long Inverted list (more than 128 postings, more than 1 block) with PForDelta.
void InvertedList::compress_long_list(uint32_t block_size, score_t idf, class Records * recs) {
	uint32_t d_gap = 0, previous_d = 0, i = 0, j = 0, x = 0, adv = 0;
	uint32_t num_blocks = this->get_num_blocks(block_size), cur_block = 0;
	uint32_t * temp_ptr, * in_ptr, * out_ptr;

	score_t score = 0.0f, max_block_score = 0.0f, scores[block_size], sscores[block_size];
	score_t K = 0.0f, dl = 0.0f, scaler = 0.0f, avgdl = recs->get_avg_doc_len();

	/// Allocate space to store the compressed data
	uint32_t * encoded_docIDs = (uint32_t*)calloc(num_blocks * this->num_postings, sizeof(uint32_t));
	uint32_t * encoded_dscors = (uint32_t*)calloc(num_blocks * this->num_postings, sizeof(uint32_t));

	/// Expand the docID and frequency buffers so that they are integrals of block_size
	this->docIDs = (uint32_t *)realloc(this->docIDs, num_blocks * block_size * sizeof(uint32_t));
	this->dscors = (uint32_t *)realloc(this->dscors, num_blocks * block_size * sizeof(uint32_t));

	/// This is a long list: Initialize the skip table
	this->skip_table = (struct list_block *)malloc(num_blocks * sizeof(struct list_block));
	for (uint32_t i = 0; i < num_blocks; i++) {
		this->skip_table[i].block_size = 0;
		this->skip_table[i].dwrd = 0;
		this->skip_table[i].swrd = 0;
		this->skip_table[i].last = 0;
		this->skip_table[i].blockMax_score = 0;
		this->skip_table[i].scaler = 0.0f;
	}

	for (i = 0; i < this->num_postings; i++) {
		/// Construct a new block: We check whether a block of docIds is complete.
		/// If it is, use a block of BLOCK_SIZE size, and encode it by using PFOR-DELTA.
		if (x >= block_size) {
			// printf("Block %d - Start Posting: %d, End Posting: %d\n", cur_block + 1, adv, i);
			/// Convert the float scores to integers so that they can be compressed.
			scaler = this->compute_score_scaler(block_size, sscores);
			// scaler = LOL_SCALER;

			/// Update the skip table
			this->skip_table[cur_block].block_size = block_size;
			this->skip_table[cur_block].dwrd = this->dwrd;
			this->skip_table[cur_block].swrd = this->swrd;
			this->skip_table[cur_block].last = previous_d;
			this->skip_table[cur_block].scaler = scaler;

			/// Compress the docID gaps
			in_ptr = this->docIDs + adv;
			out_ptr = encoded_docIDs + this->dwrd;
			this->dwrd += PFOR_CODER->Compression(in_ptr, out_ptr, this->num_postings);

			/// Convert the frequencies to quantized BM25 scores. Then, compress the integer scores.
			in_ptr = this->dscors + adv;
			for (j = 0; j < block_size; j++) {
				in_ptr[j] = (uint32_t) ( scaler * scores[j] );
				score = in_ptr[j] / scaler;
				if (score > max_block_score) { max_block_score = score; }
				if (score > this->listMax_score) { this->listMax_score = score; }
			}
			out_ptr = encoded_dscors + this->swrd;
			this->swrd += PFOR_CODER->Compression(in_ptr, out_ptr, this->num_postings);

			/// Update the block's max score in the skip table.
			this->skip_table[cur_block].blockMax_score = max_block_score;

			adv += block_size;
			cur_block += 1;

			x = 0;
			max_block_score = 0;
		}

		/// Get the doc length. Compute the posting score. Set the maximum BM25 score in the block.
		dl = (score_t)recs->get_doc_len(this->docIDs[i] - 1);
		K = BM25_k2_PARAM * (1.0f - BM25_b_PARAM + BM25_b_PARAM * dl / avgdl);
		score = idf * this->dscors[i] * (BM25_k1_PARAM + 1) / (this->dscors[i] + K);
		scores[x] = score;
		sscores[x] = score;

		// printf("\tDocID=%d, DL=%5.1f, AvgDL=%5.3f, K=%5.3f, idf=%5.3f, df=%d, score=%5.3f\n",
		//	this->docIDs[i], dl, avgdl, K, idf, this->dscors[i], score);

		/// Compute docID gaps
		d_gap = this->docIDs[i] - previous_d;
		previous_d = this->docIDs[i];
		this->docIDs[i] = d_gap;

		x++;
	}

	/// Construct the last block of the list

	/// Pad the docIDs and the scores of the last block with zeros.
	for (i = this->num_postings; i < num_blocks * block_size; i++) {
		this->docIDs[i] = 0;
		this->dscors[i] = 0;
	}

	/// Quantize the float BM25 scores so that they can be compressed. First, fill the last elements
	/// of the scores/sscores arrays with zeros, so that it is aligned with the number of postings
	/// in the last block.
	for (j = i - adv; j < block_size; j++) {
		scores[j] = 0;
		sscores[j] = 0;
	}
	scaler = this->compute_score_scaler(block_size, sscores);
	// scaler = LOL_SCALER;

	/// Update the skip table
	this->skip_table[cur_block].block_size = x;
	this->skip_table[cur_block].dwrd = this->dwrd;
	this->skip_table[cur_block].swrd = this->swrd;
	this->skip_table[cur_block].last = previous_d;
	this->skip_table[cur_block].scaler = scaler;

	/// Compress the docID gaps
	in_ptr = this->docIDs + adv;
	out_ptr = encoded_docIDs + this->dwrd;
	this->dwrd += PFOR_CODER->Compression(in_ptr, out_ptr, this->num_postings);
	encoded_docIDs = (uint32_t *)realloc(encoded_docIDs, this->dwrd * sizeof(uint32_t));
	temp_ptr = this->docIDs;
	this->docIDs = encoded_docIDs;
	free(temp_ptr);

	/// Convert the frequencies to quantized BM25 scores. Then, compress the integer scores.
	in_ptr = this->dscors + adv;
	for (j = 0; j < block_size; j++) {
		in_ptr[j] = (uint32_t) ( scaler * scores[j] );
		score = in_ptr[j] / scaler;
		if (score > max_block_score) { max_block_score = score; }
		if (score > this->listMax_score) { this->listMax_score = score; }
	}
	out_ptr = encoded_dscors + this->swrd;
	this->swrd += PFOR_CODER->Compression(in_ptr, out_ptr, this->num_postings);
	encoded_dscors = (uint32_t *)realloc(encoded_dscors, this->swrd * sizeof(uint32_t));
	temp_ptr = this->dscors;
	this->dscors = encoded_dscors;
	free(temp_ptr);

	/// Update the last block's max score in the skip table.
	this->skip_table[cur_block].blockMax_score = max_block_score;

	/// Now compute the non-decreasing blockMax scores: This improves the pivoting step of BMW.
	for (int32_t b = num_blocks - 1; b > -1; b--) {
		this->skip_table[b].blockMax_score = (b == num_blocks - 1) ?
			this->skip_table[b].blockMax_score :
			fmaxf(this->skip_table[b].blockMax_score, this->skip_table[b + 1].blockMax_score);
	}

}

/// Compress an Inverted List by creating and attaching an InvertedListHandler object
void InvertedList::compress(uint32_t block_size, score_t idf, class Records * recs) {
	this->dwrd = 0;
	this->swrd = 0;

	/// Handle the short lists
	if (this->get_num_blocks(block_size) == 1) {
		this->compress_short_list(idf, recs);

	/// Handle the long lists (more than block_size postings)
	} else {
		this->compress_long_list(block_size, idf, recs);
	}
}

/// Decompress a short Inverted list (fewer than 129 postings, 1 block) with Variable Byte.
void InvertedList::decompress_short_list() {
	uint32_t prev_d = 0;
	uint32_t * in_ptr, * out_ptr;

	printf("\tDecompressing short Inverted List with %d postings\n", this->num_postings);

	/// Allocate space to store the decompressed data
	uint32_t * decoded_docIDs = (uint32_t*)calloc(this->num_postings, sizeof(uint32_t));
	uint32_t * decoded_dscors = (uint32_t*)calloc(this->num_postings, sizeof(uint32_t));

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
	/// Update the Inverted List dscor pointer to "look" into the decompressed document frequencies.
	in_ptr = this->dscors;
	out_ptr = decoded_dscors;
	VBYTE_CODER->Decompression(in_ptr, out_ptr, this->num_postings);
	free(this->dscors);
	this->dscors = decoded_dscors;
}

/// Decompress a long Inverted list (more than 128 postings, more than 1 block) with PForDelta.
void InvertedList::decompress_long_list(uint32_t block_size) {
	uint32_t i = 0, j = 0, prev_d = 0;
	uint32_t * in_ptr, * out_ptr;
	uint32_t num_blocks = this->get_num_blocks(block_size);

	printf("\tInverted List Organization:\n\t\tPostings = %d, Blocks = %d, Block Size = %d\n",
		this->num_postings, num_blocks, block_size);

	uint32_t * decoded_docIDs = (uint32_t *)calloc(num_blocks * block_size, sizeof(uint32_t));
	uint32_t * decoded_dscors = (uint32_t *)calloc(num_blocks * block_size, sizeof(uint32_t));

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

	/// Decompress the document scores
	for (i = 0; i < num_blocks; i++) {
		in_ptr = this->dscors + this->skip_table[i].swrd;
		out_ptr = decoded_dscors + i * block_size;
		PFOR_CODER->Decompression(in_ptr, out_ptr, block_size);
	}

	/// Update the Inverted List docID pointer to "look" into the decompressed docID gaps.
	free(this->docIDs);
	this->docIDs = decoded_docIDs;

	/// Update the Inverted List dscor pointer to "look" into the decompressed document frequencies.
	free(this->dscors);
	this->dscors = decoded_dscors;
}

/// Decompress the entire inverted list. The compressed data is stored in this->docIDs and
/// this->dscor. After decompression, this->docIDs and this->dscor point to the decoded data.
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
	fwrite(&this->swrd, sizeof(uint32_t), 1, fp);

	/// If this is a long list, write the skip table to disk
	if (num_blocks > 1) {
		for (uint32_t i = 0; i < num_blocks; i++) {
			fwrite(&this->skip_table[i].block_size, sizeof(uint32_t), 1, fp);
			fwrite(&this->skip_table[i].last, sizeof(uint32_t), 1, fp);
			fwrite(&this->skip_table[i].dwrd, sizeof(uint32_t), 1, fp);
			fwrite(&this->skip_table[i].swrd, sizeof(uint32_t), 1, fp);
			fwrite(&this->skip_table[i].blockMax_score, sizeof(score_t), 1, fp);
			fwrite(&this->skip_table[i].scaler, sizeof(score_t), 1, fp);

			//printf("Block: %d, size: %d, last docID: %d, dwrd: %d, swrd: %d, Max Score: %d, Scaler: %5.3f\n",
			//	i + 1, this->skip_table[i].block_size, this->skip_table[i].last,
			//	this->skip_table[i].dwrd, this->skip_table[i].swrd, this->skip_table[i].blockMax_score,
			//	this->skip_table[i].scaler);
		}
	}

	/// Write the compressed docID gaps and document frequencies.
	fwrite(this->docIDs, sizeof(uint32_t), this->dwrd, fp);
	fwrite(this->dscors, sizeof(uint32_t), this->swrd, fp);
	fwrite(&this->listMax_score, sizeof(score_t), 1, fp);
}

/// Read an inverted list from disk
void InvertedList::read(FILE * fp) {
	uint32_t num_blocks = 0;
	size_t nread = 0;

	nread = fread(&this->num_postings, sizeof(uint32_t), 1, fp);
	if (nread == 0) {
		fprintf(stderr, "Unexpected end of inverted file\n");
		exit(-1);
	}

	nread = fread(&num_blocks, sizeof(uint32_t), 1, fp);
	nread = fread(&this->dwrd, sizeof(uint32_t), 1, fp);
	nread = fread(&this->swrd, sizeof(uint32_t), 1, fp);

	this->skip_table = NULL;

	/// If this is a long list, read the skip table from disk
	if (num_blocks > 1) {

		/// Initialize the skip table
		this->skip_table = (struct list_block *)malloc(num_blocks * sizeof(struct list_block));

		for (uint32_t i = 0; i < num_blocks; i++) {
			nread = fread(&this->skip_table[i].block_size, sizeof(uint32_t), 1, fp);
			nread = fread(&this->skip_table[i].last, sizeof(uint32_t), 1, fp);
			nread = fread(&this->skip_table[i].dwrd, sizeof(uint32_t), 1, fp);
			nread = fread(&this->skip_table[i].swrd, sizeof(uint32_t), 1, fp);
			nread = fread(&this->skip_table[i].blockMax_score, sizeof(score_t), 1, fp);
			nread = fread(&this->skip_table[i].scaler, sizeof(score_t), 1, fp);

			//printf("Block: %d, size: %d, last docID: %d, dwrd: %d, swrd: %d, Max Score: %5.3f, Scaler: %5.3f\n",
			//	i + 1, this->skip_table[i].block_size, this->skip_table[i].last,
			//	this->skip_table[i].dwrd, this->skip_table[i].swrd, this->skip_table[i].blockMax_score,
			//	this->skip_table[i].scaler);
		}
	}

	this->docIDs = (uint32_t *)calloc(this->dwrd, sizeof(uint32_t));
	this->dscors = (uint32_t *)calloc(this->swrd, sizeof(uint32_t));

	/// Read the compressed docID gaps and document frequencies.
	nread = fread(this->docIDs, sizeof(uint32_t), this->dwrd, fp);
	nread = fread(this->dscors, sizeof(uint32_t), this->swrd, fp);
	nread = fread(&this->listMax_score, sizeof(score_t), 1, fp);
}

/// Display the inverted list contents - Decompression takes place on the fly
void InvertedList::display() {
	this->decompress(128);

	printf("[ ");
	for (uint32_t i = 0; i < this->num_postings; i++) {
		printf("P-%d: (%d, %d) ", i + 1, this->docIDs[i], this->dscors[i]);
	}
	printf("]");
	fflush(NULL);
}

void InvertedList::display_skip_table(uint32_t block_size) {
	uint32_t num_blocks = this->get_num_blocks(block_size);

	/// If this is a long list, write the skip table to disk
	if (num_blocks > 1) {
		for (uint32_t i = 0; i < num_blocks; i++) {
			printf("Block: %d, size: %d, last docID: %d, dwrd: %d, swrd: %d, Scaler: %5.5f, Max Score: %5.5f\n",
				i + 1, this->skip_table[i].block_size, this->skip_table[i].last,
				this->skip_table[i].dwrd, this->skip_table[i].swrd, this->skip_table[i].scaler,
				this->skip_table[i].blockMax_score);
		}
	} else {
		printf("Short List with one block. Max list score=%5.3f\n", this->listMax_score);
	}
}

/// Comparator callback function for qsorting doc score buffers.
int InvertedList::cmp_scores(const void *a, const void *b) {
	score_t x = *(score_t *)a;
	score_t y = *(score_t *)b;

	if (x > y) {
		return 1;
	}
	return -1;
}

/// Compute the memory footprint of the inverted list
uint32_t InvertedList::get_footprint(uint32_t block_size) {
	uint32_t num_blocks = this->get_num_blocks(block_size);

	/// Sizeof the compressed inverted list data
	uint32_t footprint = sizeof(InvertedList) + (this->dwrd + this->swrd) * sizeof(uint32_t);

	/// If the list is long, also consider the memory occupied by the skip structure.
	if (num_blocks > 1) {
		footprint += num_blocks * sizeof(InvertedList::list_block);
	}
	return footprint;
}

/// Accessors
inline uint32_t * InvertedList::get_docIDs() { return this->docIDs; }
inline uint32_t * InvertedList::get_dscors() { return this->dscors; }
inline uint32_t InvertedList::get_num_postings() { return this->num_postings; }
inline score_t InvertedList::get_listMax_score() { return this->listMax_score; }
inline uint32_t InvertedList::get_dwrd() { return this->dwrd; }
inline uint32_t InvertedList::get_swrd() { return this->swrd; }
inline uint32_t InvertedList::get_list_block_size() { return sizeof(InvertedList::list_block); }

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
