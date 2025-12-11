/// ICDR: Indexed Contrastive Data Retriever

/// Entity Header File: An object used to store a single entity
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_ENTITY_H
#define ICDS_ENTITY_H

class Entity {
	uint32_t id;
	char * code;
	uint32_t num_alloc_matching_records;
	uint32_t num_matching_records;
	uint32_t *matching_records;
	class Entity * next;

	public:
		Entity();
		explicit Entity(uint32_t, char *);
		~Entity();

		void display();
		void insert_matching_record(uint32_t);
		class Record * get_matching_record_obj(class Record **, uint32_t);
		void write(FILE *);
		void read(FILE *);

		void set_id(uint32_t);
		void set_code(char *);
		void set_next(class Entity *);

		uint32_t get_id();
		char * get_code();
		uint32_t get_num_matching_records();
		uint32_t get_num_alloc_matching_records();
		class Entity * get_next();
};

#endif //ICDS_ENTITY_H
