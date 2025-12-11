/// ICDR: Indexed Contrastive Data Retriever

/// Result Implementation File: An object used to represent the result object of a query.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#include "Result.h"

/// Default Constructor
Result::Result() :
	docID(0),
	text(NULL),
	score(0.0) {
}

/// Constructor
Result::Result(uint32_t did, char * txt) :
	docID(did),
	text(txt),
	score(0.0) {
}

/// Copy Constructor
Result::Result(const Result&s) {
	printf("Copy constructor for class Result has been called"); fflush(NULL);
}


/// Destructor
Result::~Result() {
}

/// Accessor: Return the score for this result
inline score_t Result::get_score() { return this->score; }

/// Accessor: Return the document ID for this result
inline uint32_t Result::get_docID() { return this->docID; }

/// Accessor: Return the document ID for this result
inline char * Result::get_text() { return this->text; }

/// Mutator: Set the score for this result
inline void Result::set_score (double sc) { this->score = sc; }
