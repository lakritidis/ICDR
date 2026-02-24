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
	w(NULL),
	ivl(NULL),
	idf(0.0),
	next(NULL) {
}

/// Constructor
Word::Word(char * wrd) :
	w(new char[strlen(wrd) + 1]),
	ivl(new InvertedList(1)),
	idf(0.0),
	next(NULL) {
		strcpy(this->w, wrd);
}

/// Destructor
Word::~Word() {
	if (this->w) {
		delete [] this->w;
	}

	if (this->ivl) {
		delete this->ivl;
	}
}

/// Display the properties of a Word object
void Word::display() {
	printf("\n\nWord: %s, IDF: %5.3f, Inverted List:\n", this->w, this->idf); fflush(NULL);
	this->ivl->display();
}

/// Write the Word to a file
void Word::write(FILE * fp) {
	uint32_t word_len = strlen(this->w);

	fwrite(&word_len, sizeof(uint32_t), 1, fp);
	fwrite(this->w, sizeof(char), word_len, fp);
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
	return sizeof(Word) + (strlen(w) + 1) * sizeof(char) + this->ivl->get_footprint(block_size);
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
void Word::set_word_string(char * v) { this->w = v; }
void Word::set_ivl(class InvertedList * v) { this->ivl = v; }
void Word::set_next(class Word * v) { this->next = v; }
void Word::set_idf(score_t v) { this->idf = v; }

/// Accessors
inline char * Word::get_str() { return this->w; }
inline class InvertedList * Word::get_ivl() { return this->ivl; }
inline class Word * Word::get_next() { return this->next; }
inline score_t Word::get_idf() { return this->idf; }

uint32_t Word::get_freq() { return this->ivl->get_num_postings(); }

#endif
