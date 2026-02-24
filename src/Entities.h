/**
ICDR: Indexed Contrastive Data Retriever

Entities header file: A hash table that accommodates distinct entities.

L. Akritidis, 2026
*/

#ifndef ICDR_ENTITIES_H
#define ICDR_ENTITIES_H

class Entities {
	class Entity ** hash_table;
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
		class Entity * get_entity(char *);
		void insert(class Entity *);
		void write(FILE *);
		void read(FILE *);
		void display();
		void compute_stats();

		/// Accessors
		uint32_t get_num_chains();
		uint32_t get_num_slots();
		uint32_t get_num_nodes();
		class Entity * get_table_entry(uint32_t);
};

#endif // ICDR_ENTITIES_H
