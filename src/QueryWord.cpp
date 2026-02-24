/**
ICDR: Indexed Contrastive Data Retriever

QueryWord implementation file: An object used to represent a word (term or token) of a query. It
extends the Word class.

L. Akritidis, 2026
*/

#ifndef ICDR_QUERYWORD_CPP
#define ICDR_QUERYWORD_CPP

#include "QueryWord.h"

/// Default constructor
QueryWord::QueryWord() : Word(),
	ivl_it(NULL),
	q_pos(0) {
}

/// Default constructor
QueryWord::QueryWord(class Word * wrd, uint32_t block_size) :
	Word(),
	ivl_it(new InvertedListIterator(wrd->get_ivl(), block_size)),
	q_pos(0) {

		this->w = new char[strlen(wrd->get_str()) + 1];
		strcpy(this->w, wrd->get_str());

		this->ivl = NULL;
}

/// Destructor
QueryWord::~QueryWord() {
	if (this->ivl_it) {
		delete this->ivl_it;
	}
}

/// Accessors
inline InvertedListIterator * QueryWord::get_ivl_it() { return this->ivl_it; }
inline InvertedListIterator * QueryWord::get_c_ivl_it() const { return this->ivl_it; }
#endif
