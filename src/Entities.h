/// ICDR: Indexed Contrastive Data Retriever

/// Entities Header File: A hash table that accommodates distinct entities.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_ENTITIES_H
#define ICDS_ENTITIES_H

class Entities {
	class Entity ** hash_table;
	class Entity ** table;
	uint32_t mask;
	uint32_t num_slots;  /// The number of slots of the hash table
	uint32_t num_nodes;	 /// The number of elements stored in the hash table.
	uint32_t num_chains; /// The number of non-empty chains.

	char * ent_file;

	private:
		uint32_t djb2(char *);

    public:
		Entities();
		Entities(uint32_t);
		~Entities();

		class Entity * insert(char *, uint32_t);
		void insert(class Entity *);
		void write(FILE *);
		void read(FILE *);
		void convert_to_array();
		void display();
		void display_list();
};

#endif // ICDS_ENTITIES_H
