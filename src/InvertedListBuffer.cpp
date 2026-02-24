/**
ICDR: Index-Assisted Contrastive Data Synthesizer

InvertedListBuffer implementation file: An object used during index construction to accumulate
postings. When BLOCK_SIZE postings have been accumulated, the postings are compressed on-the-fly
and written in the compressed buffers of the corresponding InvertedList.

L. Akritidis, 2026
*/

#ifndef INVERTEDLISTBUFFER_CPP
#define INVERTEDLISTBUFFER_CPP

#include "InvertedListBuffer.h"

InvertedListBuffer::InvertedListBuffer() :
	wrd(NULL),
	num_postings(0),
	num_alloc_postings(0),
	docIDs(NULL),
	dfreqs(NULL) {
}

InvertedListBuffer::InvertedListBuffer(class Word * w) :
	wrd(w),
	num_postings(0),
	num_alloc_postings(1),
	docIDs(NULL),
	dfreqs(NULL) {
		this->docIDs = (uint32_t *)malloc(this->num_alloc_postings * sizeof(uint32_t));
		this->dfreqs = (uint32_t *)malloc(this->num_alloc_postings * sizeof(uint32_t));
}

InvertedListBuffer::~InvertedListBuffer() {
	if (this->docIDs) {
		free(this->docIDs);
	}

	if (this->dfreqs) {
		free(this->dfreqs);
	}
}

uint32_t InvertedListBuffer::insert_posting(uint32_t d) {
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

	// class InvertedList * il = this->wrd->get_ivl();
	return result;
}

#endif
