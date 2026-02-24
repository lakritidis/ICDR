/**
ICDR: Indexed Contrastive Data Retriever

Entities implementation file: A hash table that accommodates distinct Entity objects.

L. Akritidis, 2026
*/

#ifndef ICDR_ENTITIES_CPP
#define ICDR_ENTITIES_CPP

#include "Entity.cpp"
#include "Entities.h"

/// Constructor 1: default
Entities::Entities() :
		hash_table(NULL),
		num_slots(0),
		num_nodes(0),
		num_chains(0),
		ent_file(NULL) {
}

/// Constructor 2: overloaded
Entities::Entities(uint32_t size) :
		hash_table(new Entity * [size]),
		num_slots(size),
		num_nodes(0),
		num_chains(0),
		ent_file(NULL) {
			for (uint32_t i = 0; i < size; i++) {
				this->hash_table[i] = NULL;
			}
}

/// Destructor
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

	this->num_nodes = 0;
	this->num_chains = 0;
}

/// The DJB2 Hash Function (Dan Bernstein)
uint32_t Entities::djb2(char * str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

/// Insert an element into the hash table
class Entity * Entities::insert(char * ent_code, uint32_t matching_record_id) {
	/// Find the hash value of the input term
	uint32_t HashValue = this->djb2(ent_code) % this->num_slots;

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
	uint32_t HashValue = this->djb2(ent_code) % this->num_slots;

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

/// Search for an Entity object in the hash table, given its code.
class Entity * Entities::get_entity(char * ent_code) {
	uint32_t HashValue = this->djb2(ent_code) % this->num_slots;

	/// Now search in the hash table to check whether this term exists or not
	if (this->hash_table[HashValue] != NULL) {
		class Entity * q;

		/// Traverse the linked list that represents the chain.
		for (q = this->hash_table[HashValue]; q != NULL; q = q->get_next()) {
			if (strcmp(q->get_code(), ent_code) == 0) {
				return q; /// Return and exit
			}
		}
	} else {
		this->num_chains++;
	}
	return NULL;
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
	size_t nread = 0;

	if (fp) {
		nread = fread(&num_entities, sizeof(uint32_t), 1, fp);
		if (nread == 0) {
			fprintf(stderr, "Unexpected end of Entities file\n");
			exit(-1);
		}

		// printf("Num entities: %d", num_records); fflush(NULL);
		for (uint32_t i = 0; i < num_entities; i++) {
			q = new Entity();
			q->read(fp);

			this->insert(q);
		}
	} else {
		fprintf(stderr, "Error reading Entities file\n");
		exit(-1);
	}
}

/// Display the stored Entities
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

/// Compute statistics for the Entities structure
void Entities::compute_stats() {
	class Entity * q;
	uint32_t footprint = sizeof(class Entity **) + this->num_slots * sizeof(class Entity *);

	for (uint32_t i = 0; i < this->num_slots; i++) {
		if (this->hash_table[i] != NULL) {
			for (q = this->hash_table[i]; q != NULL; q = q->get_next()) {
				footprint += q->get_footprint();
			}
		}
	}

	printf(" === Entities statistics ========================== \n");
	printf("\tHash table slots: %d\n", this->num_slots);
	printf("\tNumber of Entities: %d\n", this->num_nodes);
	printf("\tMemory footprint: %5.2f MB\n", footprint / 1048576.0f);
	printf(" ================================================== \n\n");
}

/// Accessors
class Entity * Entities::get_table_entry(uint32_t i) { return this->hash_table[i]; }
uint32_t Entities::get_num_chains() { return this->num_chains; }
uint32_t Entities::get_num_slots() { return this->num_slots; }
uint32_t Entities::get_num_nodes() { return this->num_nodes; }
#endif
