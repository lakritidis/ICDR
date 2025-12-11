/// ICDR: Indexed Contrastive Data Retriever

/// Entity Implementation File: An object used to store a single entity
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_ENTITY_CPP
#define ICDS_ENTITY_CPP

#include "Entity.h"

/// Entity default constructor
Entity::Entity() :
	id(0),
	code(NULL),
	num_alloc_matching_records(0),
	num_matching_records(0),
	matching_records(NULL),
	next(NULL) {

	}

/// Entity constructor
Entity::Entity(uint32_t i, char c[]) :
	id(i),
	code(NULL),
	num_alloc_matching_records(2),
	num_matching_records(0),
	matching_records(NULL),
	next(NULL) {
		this->matching_records = (uint32_t *)malloc(this->num_alloc_matching_records * sizeof(uint32_t));

		this->code = new char[strlen(c) + 1];
		strcpy(this->code, c);
	}

/// Entity Destructor
Entity::~Entity() {
	if (this->code) {
		delete [] this->code;
	}
	free(this->matching_records);
};

/// Display the member variables of an Entity object
void Entity::display() {
	printf("==================\n");
	printf("Displaying Entity with ID: %d \n", this->id);
	printf("\tID: %d, Matching Records: %u, Code: %s\n", this->id, this->num_matching_records, this->code);
	printf("\tMatching_record IDs: ");
	for (uint32_t i = 0; i < this->num_matching_records; i++) {
		printf("%d, ", this->matching_records[i]);
	}
	printf("\n");
	fflush(NULL);
}

/// Insert a Record ID into the cluster of matching records
void Entity::insert_matching_record(uint32_t v) {

	this->matching_records[this->num_matching_records++] = v;

	if (this->num_matching_records >= this->num_alloc_matching_records) {
		this->num_alloc_matching_records *= 2;
		this->matching_records = (uint32_t *)realloc(this->matching_records, this->num_alloc_matching_records * sizeof(uint32_t));
	}
}

/// Write an Entity object to disk (file fp)
void Entity::write(FILE * fp) {
	uint32_t len = strlen(this->code);

	fwrite(&this->id, sizeof(uint32_t), 1, fp);
	fwrite(&len, sizeof(uint32_t), 1, fp);
	fwrite(this->code, sizeof(char), len, fp);

	fwrite(&this->num_matching_records, sizeof(uint32_t), 1, fp);
	fwrite(this->matching_records, sizeof(uint32_t), this->num_matching_records, fp);
}

/// Read an Entity object from disk (file fp)
void Entity::read(FILE * fp) {
	uint32_t len = 0;

	fread(&this->id, sizeof(uint32_t), 1, fp);
	fread(&len, sizeof(uint32_t), 1, fp);
	if (len > 0) {
		this->code = new char[len + 1];
		fread(this->code, sizeof(char), len, fp);
		this->code[len] = 0;
	} else {
		this->code = NULL;
	}
	fread(&this->num_matching_records, sizeof(uint32_t), 1, fp);

	this->num_alloc_matching_records = this->num_matching_records;
	this->matching_records = (uint32_t *)malloc(this->num_matching_records * sizeof(uint32_t));

	fread(this->matching_records, sizeof(uint32_t), this->num_matching_records, fp);

	this->next = NULL;
}

/// Mutators
void Entity::set_id(uint32_t v) { this->id = v; }
void Entity::set_next(class Entity * v) { this->next = v; }
void Entity::set_code(char * v) {
	this->code = new char[strlen(v) + 1];
	strcpy(this->code, v);
}

/// Accessors
inline uint32_t Entity::get_id() { return this-> id; }
inline char * Entity::get_code() { return this->code; }
inline uint32_t Entity::get_num_matching_records() { return this->num_matching_records; }
inline class Entity * Entity::get_next() { return this->next; }
inline class Record * get_matching_record_obj(class Record ** recs, uint32_t idx) { return recs[idx]; }

#endif
