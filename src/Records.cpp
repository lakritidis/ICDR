/// ICDR: Indexed Contrastive Data Retriever

/// Records Implementation File: An array that accommodates distinct Record objects.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_RECORDS_CPP
#define ICDS_RECORDS_CPP

#include "Records.h"
#include "Record.cpp"

/// Default Constructor
Records::Records() :
	num_records(0),
	num_alloc_records(0),
	records(NULL),
	avg_doc_len(0.0),
	total_num_words(0) {
}

/// Constructor
Records::Records(uint32_t k) :
	num_records(0),
	num_alloc_records(k),
	records(NULL),
	avg_doc_len(0.0),
	total_num_words(0) {
		this->records = (class Record **)malloc(this->num_alloc_records * sizeof(class Record *));
}

/// Destructor
Records::~Records() {
	for (uint32_t i = 0; i < this->num_records; i++) {
		delete this->records[i];
	}
	free(this->records);
}

/// Create a Record object and insert it into the array
void Records::insert(uint32_t rec_id, char * rec_title, class Entity * matching_entity) {
	Record * rec = new Record(rec_id, rec_title, matching_entity);
	this->records[this->num_records++] = rec;

	if (this->num_records >= this->num_alloc_records) {
		this->num_alloc_records *= 2;
		this->records = (class Record **) realloc(this->records,
				this->num_alloc_records * sizeof(class Record *));
	}
}


/// Write the Records to disk
void Records::write(FILE * fp) {
	if (fp) {
		fwrite(&this->num_records, sizeof(uint32_t), 1, fp);
		fwrite(&this->avg_doc_len, sizeof(score_t), 1, fp);
		fwrite(&this->total_num_words, sizeof(uint32_t), 1, fp);

		for (uint32_t i = 0; i < this->num_records; i++) {
			// ret->records[i]->display();
			this->records[i]->write(fp);
		}
	} else {
		printf("Error writing Records file..."); fflush(NULL);
	}
}

/// Read the Records from disk
void Records::read(FILE * fp) {
	if (fp) {
		fread(&this->num_records, sizeof(uint32_t), 1, fp);
		fread(&this->avg_doc_len, sizeof(score_t), 1, fp);
		fread(&this->total_num_words, sizeof(uint32_t), 1, fp);
		this->num_alloc_records = this->num_records;

		this->records = (class Record **)malloc(this->num_records * sizeof(Record *));
		for (uint32_t i = 0; i < this->num_records; i++) {
			this->records[i] = new Record();
			this->records[i]->read(fp);
			// ret->records[i]->display();
		}
	} else {
		printf("Error reading Records file..."); fflush(NULL);
	}
}

/// Accessors
uint32_t Records::get_id(uint32_t k) { return this->records[k]->get_id(); }
char * Records::get_text(uint32_t k) { return this->records[k]->get_text(); }

uint32_t Records::get_num_records() { return this->num_records; }
class Record ** Records::get_records() { return this->records; }
class Record * Records::get_record(uint32_t i) { return this->records[i]; }
score_t Records::get_avg_doc_len() { return this->avg_doc_len; }

/// Mutators
void Records::set_avg_doc_len(score_t v) { this->avg_doc_len = v; }
void Records::set_total_num_words(uint32_t v) {
	this->total_num_words = v;
	this->avg_doc_len = (score_t)this->total_num_words / (score_t)this->num_records;
}
#endif
