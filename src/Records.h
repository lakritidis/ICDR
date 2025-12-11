/// ICDR: Indexed Contrastive Data Retriever

/// Records header File: An array that accommodates distinct Record objects.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RECORDS_H
#define RECORDS_H


class Records {
	uint32_t num_records;
	uint32_t num_alloc_records;
	class Record ** records;

	score_t avg_doc_len;
	uint32_t total_num_words;

	public:
		Records();
		Records(uint32_t);
		~Records();

		void insert(uint32_t, char *, class Entity *);
		void write(FILE *);
		void read(FILE *);

		uint32_t get_id(uint32_t);
		char * get_text(uint32_t);
		uint32_t get_num_records();
		class Record ** get_records();
		class Record * get_record(uint32_t);
		score_t get_avg_doc_len();

		/// Mutators
		void set_avg_doc_len(score_t);
		void set_total_num_words(uint32_t);
};

#endif // RECORDS_H
