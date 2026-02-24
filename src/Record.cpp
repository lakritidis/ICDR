/**
ICDR: Indexed Contrastive Data Retriever

Record implementation file: An object used to represent an input record. An inverted index
is constructed on the titles of these records. Given a query, ICDR will return appropriate
matching or non-matching records.

L. Akritidis, 2026
*/

#ifndef ICDR_RECORD_CPP
#define ICDR_RECORD_CPP

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
	printf("ID: %u, TEXT: %s, LENGTH (WORDS): %d (%d unique), MATCHING ENTITY: %d.\n",
		this->id, this->text, this->word_len, this->uword_len, this->matching_entity->get_id());
}

/// Write the Record to disk (file fp)
void Record::write(FILE * fp) {
	uint32_t len = strlen(this->text);
	char * matching_entity_code = this->matching_entity->get_code();

	fwrite(&this->id, sizeof(uint32_t), 1, fp);
	fwrite(&len, sizeof(uint32_t), 1, fp);
	fwrite(this->text, sizeof(char), len, fp);
	fwrite(&this->word_len, sizeof(uint32_t), 1, fp);
	fwrite(&this->word_len, sizeof(uint32_t), 1, fp);

	len = strlen(matching_entity_code);
	fwrite(&len, sizeof(uint32_t), 1, fp);
	fwrite(matching_entity_code, sizeof(char), len, fp);
}

/// Read the Record from disk (file fp)
void Record::read(FILE * fp, class Entities * ents) {
	uint32_t len = 0;
	size_t nread = 0;
	char matching_entity_code[1024];

	nread = fread(&this->id, sizeof(uint32_t), 1, fp);
	if (nread == 0) {
		fprintf(stderr, "Unexpected end of Records file\n");
		exit(-1);
	}

	nread = fread(&len, sizeof(uint32_t), 1, fp);
	this->text = new char[len + 1];
	nread = fread(this->text, sizeof(char), len, fp);
	this->text[len] = 0;

	nread = fread(&this->word_len, sizeof(uint32_t), 1, fp);
	nread = fread(&this->uword_len, sizeof(uint32_t), 1, fp);
	nread = fread(&len, sizeof(uint32_t), 1, fp);
	nread = fread(matching_entity_code, sizeof(char), len, fp);
	matching_entity_code[len] = 0;

	if (ents) {
		this->matching_entity = ents->get_entity(matching_entity_code);
	}
}

uint32_t Record::get_footprint() const {
	return sizeof(Record) + (strlen(this->text) + 1) * sizeof(char);
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
