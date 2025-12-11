/// ICDR: Indexed Contrastive Data Retriever

/// QueryWord Implementation File: An object used to represent a word (term or token) of a query.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_QUERYWORD_CPP
#define ICDS_QUERYWORD_CPP

#include "QueryWord.h"

/// Default constructor
QueryWord::QueryWord() : Word(),
	ivl_it(NULL),
	q_pos(0),
	idf(0.0) {
}

/// Default constructor
QueryWord::QueryWord(class Word * wrd, uint32_t block_size) :
	Word(),
	ivl_it(new InvertedListIterator(wrd->get_ivl(), block_size)),
	q_pos(0),
	idf(0.0) {
//		this->w = new char[strlen(wrd->get_str()) + 1];
//		strcpy(this->w, wrd->get_str());
		this->w = new char [strlen(wrd->get_str()) + 1];
		strcpy(this->w, wrd->get_str());
		this->ivl = NULL;
}

/// Destructor
QueryWord::~QueryWord() {
	delete this->ivl_it;
}

/// Accessors
inline InvertedListIterator * QueryWord::get_ivl_it() { return this->ivl_it; }
inline score_t QueryWord::get_idf() { return this->idf; }

/// Mutators
void QueryWord::set_idf(score_t v) { this->idf = v; }
#endif
