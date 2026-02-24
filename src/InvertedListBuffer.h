/**
ICDR: Index-Assisted Contrastive Data Synthesizer

InvertedListBuffer implementation file: An object used during index construction to accumulate
postings. When BLOCK_SIZE postings have been accumulated, the postings are compressed on-the-fly
and written in the compressed buffers of the corresponding InvertedList.

L. Akritidis, 2026
*/

#ifndef INVERTEDLISTBUFFER_H
#define INVERTEDLISTBUFFER_H


class InvertedListBuffer {
	class Word * wrd;
	uint32_t num_postings;
	uint32_t num_alloc_postings;
	uint32_t * docIDs;
	uint32_t * dfreqs;

	public:
		InvertedListBuffer();
		InvertedListBuffer(class Word *);
		~InvertedListBuffer();

		uint32_t insert_posting(uint32_t);
};

#endif // INVERTEDLISTBUFFER_H
