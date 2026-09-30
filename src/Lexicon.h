/**
ICDR: Indexed Contrastive Data Retriever

Lexicon implementation file: A hast table that stores the distinct words (i.e. Word objects) of
a document collection.

L. Akritidis, 2026
*/

#ifndef ICDR_LEXICON_H
#define ICDR_LEXICON_H


class Lexicon {
	char * word_buffer;
	class Word ** hash_table;

	uint32_t word_buffer_alloc_length;
	uint32_t word_buffer_length;
	uint32_t min_term_length;
	uint32_t tslots;		 /// Number of hash table buckets.
	uint32_t num_words;		 /// Number of elements stored in the hash table.
	uint32_t ColissionCount; /// Number of collisions in HashTable

	uint32_t compression_block_size;

	private:
		uint32_t KazLibHash(char *);
		uint32_t JZHash(char *, uint32_t);
		uint32_t djb2(char *);
		void expand_table();

	public:
		Lexicon(class InputParams *);
		~Lexicon();

		uint32_t insert(uint32_t, char *, char *);
		uint32_t insert(uint32_t, char *, score_t, class InvertedList *);
		class Word * search(char *, char *);

		void compress_index(class Records *);
		void write_index(FILE *);
		void read_index(FILE *, uint32_t);

		void set_avg_doc_len(score_t);
		score_t get_avg_doc_len();
		void display();
		void display_hash_table_performance();
		void compute_stats();
		char * get_word_buffer();
};

#endif // ICDR_LEXICON_H
