/// ICDR: Indexed Contrastive Data Retriever

/// Record Implementation File: An object used to represent an input record. An inverted index
/// is constructed on the titles of these records. Given a query, ICDS will return appropriate
/// matching or non-matching records.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_RECORD_CPP
#define ICDS_RECORD_CPP

#include "Entity.h"
#include "Record.h"

Record::Record() :
	id(0),
	text(NULL),
	word_len(0),
	uword_len(0),
	matching_entity(NULL) { }

Record::Record(const uint32_t i, const char * s, class Entity * e) :
				id(i),
				text(new char[strlen(s) + 1]),
				word_len(0),
				uword_len(0),
				matching_entity(e) {
	strcpy(this->text, s);
}

Record::~Record() {
	if(this->text) {
		delete [] this->text;
	}
}

/// Display a Record object
void Record::display() const {
	printf("ID: %u, Text: %s, Word Length: %d (%d unique)\n",
		this->id, this->text, this->word_len, this->uword_len);
}

/// Write the Record to disk (file fp)
void Record::write(FILE * fp) {
	uint32_t len = strlen(this->text);
	uint32_t matching_entity_id = this->matching_entity->get_id();

	fwrite(&this->id, sizeof(uint32_t), 1, fp);
	fwrite(&len, sizeof(uint32_t), 1, fp);
	fwrite(this->text, sizeof(char), len, fp);
	fwrite(&this->word_len, sizeof(uint32_t), 1, fp);
	fwrite(&this->word_len, sizeof(uint32_t), 1, fp);
	fwrite(&matching_entity_id, sizeof(uint32_t), 1, fp);
}

/// Read the Record from disk (file fp)
void Record::read(FILE * fp) {
	uint32_t len = 0, matching_entity_id = 0;

	fread(&this->id, sizeof(uint32_t), 1, fp);

	fread(&len, sizeof(uint32_t), 1, fp);
	this->text = new char[len + 1];
	fread(this->text, sizeof(char), len, fp);
	this->text[len] = 0;

	fread(&this->word_len, sizeof(uint32_t), 1, fp);
	fread(&this->uword_len, sizeof(uint32_t), 1, fp);
	fread(&matching_entity_id, sizeof(uint32_t), 1, fp);
}

/// Accessors
inline uint32_t Record::get_id() const { return this->id; }
inline char * Record::get_text() const { return this->text; }
inline uint32_t Record::get_word_len() const { return this->word_len; }
inline uint32_t Record::get_uword_len() const { return this->uword_len; }
inline Entity * Record::get_matching_entity() const { return this->matching_entity; }

/// Mutators
void Record::set_id(uint32_t v) { this->id = v; }
void Record::set_word_len(uint32_t v) { this->word_len = v; }
void Record::set_uword_len(uint32_t v) { this->uword_len = v; }

#endif
