/// ICDR: Indexed Contrastive Data Retriever

/// Entities Implementation File: A hash table that accommodates distinct Entity objects.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_ENTITIES_CPP
#define ICDS_ENTITIES_CPP

#include "Entity.cpp"
#include "Entities.h"

/// Constructor 1: default
Entities::Entities() :
		hash_table(NULL),
		table(NULL),
		mask(0),
		num_slots(0),
		num_nodes(0),
		num_chains(0),
		ent_file(NULL) {
}

/// Constructor 2: overloaded
Entities::Entities(uint32_t size) :
		hash_table(new Entity * [size]),
		table(NULL),
		mask(size - 1),
		num_slots(size),
		num_nodes(0),
		num_chains(0),
		ent_file(NULL) {
			for (uint32_t i = 0; i < size; i++) {
				this->hash_table[i] = NULL;
			}
}

Entities::~Entities() {
	class Entity * q;

	if (this->hash_table) {
		for (uint32_t i = 0; i < this->num_slots; i++) {
			while (this->hash_table[i] != NULL) {
				q = this->hash_table[i]->get_next();
				delete this->hash_table[i];
				this->hash_table[i] = q;
			}
		}

		delete [] this->hash_table;
		this->hash_table = NULL;
	}

	if (this->table) {
		delete [] this->table;
		this->table = NULL;
	}
	this->num_nodes = 0;
	this->num_chains = 0;
}

/// Insert an element into the hash table
class Entity * Entities::insert(char * ent_code, uint32_t matching_record_id) {
	/// Find the hash value of the input term
	uint32_t HashValue = this->djb2(ent_code) & this->mask;

	/// Now search in the hash table to check whether this term exists or not
	if (this->hash_table[HashValue] != NULL) {
		class Entity * q;

		/// Traverse the linked list that represents the chain.
		for (q = this->hash_table[HashValue]; q != NULL; q = q->get_next()) {
			if (strcmp(q->get_code(), ent_code) == 0) {

				q->insert_matching_record(matching_record_id);
				return q; /// Return and exit
			}
		}
	} else {
		this->num_chains++;
	}

	this->num_nodes++;

	/// Reassign the chain's head
	Entity * v = new Entity(this->num_nodes, ent_code);
	v->insert_matching_record(matching_record_id);

	v->set_next(this->hash_table[HashValue]);
	this->hash_table[HashValue] = v;

	return v;
}

/// Insert an Entity object into the hash table
void Entities::insert(class Entity * e) {
	/// Find the hash value of the input term
	char * ent_code = e->get_code();
	uint32_t HashValue = this->djb2(ent_code) & this->mask;

	/// Now search in the hash table to check whether this term exists or not
	if (this->hash_table[HashValue] != NULL) {
		class Entity * q;

		/// Traverse the linked list that represents the chain.
		for (q = this->hash_table[HashValue]; q != NULL; q = q->get_next()) {
			if (strcmp(q->get_code(), ent_code) == 0) {
				printf("Found a record with the same code!"); fflush(NULL);
				return; /// Return and exit
			}
		}
	} else {
		this->num_chains++;
	}

	this->num_nodes++;

	/// Reassign the chain's head
	e->set_next(this->hash_table[HashValue]);
	this->hash_table[HashValue] = e;
}

/// The DJB2 Hash Function (Dan Bernstein)
uint32_t Entities::djb2(char * str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

/// Write the Entities to disk
void Entities::write(FILE * fp) {
	fwrite(&this->num_nodes, sizeof(uint32_t), 1, fp);
	class Entity * q;
	for (uint32_t i = 0; i < this->num_slots; i++) {
		if (this->hash_table[i] != NULL) {
			for (q = this->hash_table[i]; q != NULL; q = q->get_next()) {
				q->write(fp);
			}
		}
	}
}

/// Read the Entities from disk
void Entities::read(FILE * fp) {
	uint32_t num_entities = 0;
	class Entity * q = NULL;

	if (fp) {
		fread(&num_entities, sizeof(uint32_t), 1, fp);
		// printf("Num entities: %d", num_records); fflush(NULL);
		for (uint32_t i = 0; i < num_entities; i++) {
			q = new Entity();
			q->read(fp);

			this->insert(q);
		}
	} else {
		printf("Error reading Entities file..."); fflush(NULL);
	}
}

/// Display the items of the MergedList object (hash_table)
void Entities::display() {
	class Entity * q;
	for (uint32_t i = 0; i < this->num_slots; i++) {
		if (this->hash_table[i] != NULL) {
			for (q = this->hash_table[i]; q != NULL; q = q->get_next()) {
				q->display();
			}
		}
	}
}

/// Display the items of the MergedList object (item_list)
void Entities::display_list() {
	for (uint32_t i = 0; i < this->num_nodes; i++) {
		this->table[i]->display();
	}
}

#endif
