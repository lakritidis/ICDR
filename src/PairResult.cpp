/**
ICDR: Indexed Contrastive Data Retriever

PairResult implementation file: An object used to represent a (usually neagtive) pair.

L. Akritidis, 2026
*/

#include "Result.cpp"
#include "PairResult.h"

/// Empty constructor
PairResult::PairResult() :
	Result(),
	left_docID(0),
	left_text(NULL) {
}

/// Constructor
PairResult::PairResult(uint32_t ldid, char * ltxt, uint32_t rdid, char * rtxt) :
	Result(rdid, rtxt),
	left_docID(ldid),
	left_text(ltxt) {
}

/// Copy Constructor
PairResult::PairResult(const PairResult&s) {
	printf("Copy constructor for class Result has been called"); fflush(NULL);
}

/// Destructor
PairResult::~PairResult() {
}

/// Display a PairResult object
void PairResult::display() {
	printf("Left: %d - %s, Right: %d - %s --- Score: %5.3f\n",
		this->left_docID, this->left_text, this->docID, this->text, this->score);
}

/// Mutators
void PairResult::set_right_docID(uint32_t v) { this->docID = v; }
void PairResult::set_right_text(char * v) { this->text = v; }
void PairResult::set_left_docID(uint32_t v) { this->left_docID = v; }
void PairResult::set_left_text(char * v) { this->left_text = v; }

/// Accessors
uint32_t PairResult::get_right_docID() { return this->docID; }
char * PairResult::get_right_text() { return this->text; }
uint32_t PairResult::get_left_docID() { return this->left_docID; }
char * PairResult::get_left_text() { return this->left_text; }
