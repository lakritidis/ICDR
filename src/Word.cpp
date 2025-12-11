/// ICDR: Indexed Contrastive Data Retriever

/// Word Implementation File: An object used to represent a word (term or token) of a document.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_WORD_CPP
#define ICDS_WORD_CPP

#include "Word.h"

Word::Word() :
	w(NULL),
	ivl(NULL),
	next(NULL) {
}

Word::Word(char * wrd) :
	w(new char[strlen(wrd) + 1]),
	ivl(new InvertedList(1)),
	next(NULL) {
		strcpy(this->w, wrd);
}

Word::~Word() {
	if (this->w) {
		delete [] this->w;
	}

	if (this->ivl) {
		delete this->ivl;
	}
}

void Word::display() {
	printf("\n\nWord: %s, Inverted List:\n", this->w); fflush(NULL);
	this->ivl->display();
}

void Word::write(FILE * fp) {
	uint32_t word_len = strlen(this->w);

	fwrite(&word_len, sizeof(uint32_t), 1, fp);
	fwrite(this->w, sizeof(char), word_len, fp);
}

uint32_t Word::insert_posting(uint32_t d) {
	return this->ivl->insert_posting(d);
}

void Word::compress_list(uint32_t block_size) {
	this->ivl->compress(block_size);
}

void Word::write_list(FILE * fp, uint32_t block_size) {
	this->ivl->write(fp, block_size);
}

void Word::read_list(FILE * fp) {
	this->ivl->read(fp);
}

void Word::set_word_string(char * v) { this->w = v; }
void Word::set_ivl(class InvertedList * v) { this->ivl = v; }
void Word::set_next(class Word * v) { this->next = v; }

inline char * Word::get_str() { return this->w; }
inline class InvertedList * Word::get_ivl() { return this->ivl; }
inline class Word * Word::get_next() { return this->next; }

uint32_t Word::get_freq() {
	return this->ivl->get_num_postings();
}
#endif
