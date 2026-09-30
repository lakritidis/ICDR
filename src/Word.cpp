/**
ICDR: Indexed Contrastive Data Retriever

Word implementation file: An object used to represent a word (term or token) of a document.

L. Akritidis, 2026
*/

#ifndef ICDR_WORD_CPP
#define ICDR_WORD_CPP

#include "Word.h"

/// Default constructor
Word::Word() :
	offset(0),
	idf(0.0),
	ivl(NULL) {
}

/// Constructor
Word::Word(uint32_t ofs) :
	offset(ofs),
	idf(0.0),
	ivl(new InvertedList(1)) {
}

/// Destructor
Word::~Word() {
	if (this->ivl) {
		delete this->ivl;
	}
}

/// Display the properties of a Word object
void Word::display(char * word_buffer) {
	char tmp[MAX_TERM_LENGTH];
	this->get_word_string(word_buffer, tmp);
	printf("\n\nWord: %s, IDF: %5.3f, Inverted List:\n", tmp, this->idf); fflush(NULL);
	this->ivl->display();
}

/// Write the Word to a file
void Word::write(FILE * fp) {
	fwrite(&this->offset, sizeof(uint32_t), 1, fp);
	fwrite(&this->idf, sizeof(score_t), 1, fp);
}

/// Insert a (decompressed) posting into the list
uint32_t Word::insert_posting(uint32_t d) {
	return this->ivl->insert_posting(d);
}

/// Compress the Word's inverted list by invoking its compress method
void Word::compress_list(uint32_t block_size, class Records * recs) {
	this->ivl->compress(block_size, this->idf, recs);
}

/// Compute the memory footprint of a Word object (+ its inverted list)
uint32_t Word::get_footprint(uint32_t block_size) {
	return sizeof(Word) + this->ivl->get_footprint(block_size);
}

/// Write the Word's inverted list to a file
void Word::write_list(FILE * fp, uint32_t block_size) {
	this->ivl->write(fp, block_size);
}

/// Read the Word's inverted list from a file
void Word::read_list(FILE * fp) {
	this->ivl->read(fp);
}

/// Mutators
void Word::set_offset(uint32_t v) { this->offset = v; }
void Word::set_ivl(class InvertedList * v) { this->ivl = v; }
void Word::set_idf(score_t v) { this->idf = v; }

/// Retrieve the word string from the large Lexicon `word_buffer`. Store the string in `ret_buffer`.
void Word::get_word_string(char * word_buffer, char * ret_buffer) {
	uint32_t ofs = this->offset, x = 0;
	char c = word_buffer[ofs];

	/// Start reading from the position `ofs` until a zero termination character is found.
	while (c != 0) {
		c = word_buffer[ofs++];
		ret_buffer[x++] = c;
		//printf("%d. %s|\n", x, ret_buffer);
		if (x >= MAX_TERM_LENGTH) {
			break;
		}
	}
	ret_buffer[x] = 0;
	//printf("Offset: %d, ret_buffer: %s", this->offset, ret_buffer); getchar();
}

/// Accessors
inline class InvertedList * Word::get_ivl() { return this->ivl; }
uint32_t Word::get_offset() { return this->offset; }
inline score_t Word::get_idf() { return this->idf; }

uint32_t Word::get_freq() { return this->ivl->get_num_postings(); }

#endif
