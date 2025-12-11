/// ICDR: Indexed Contrastive Data Retriever

/// Lexicon Implementation File: A hast table that stores the distinct words (i.e. Word objects)
/// of a document collection.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_LEXICON_H
#define ICDS_LEXICON_H


class Lexicon {
	class Word **hash_table;

	uint32_t min_term_length;
	uint32_t tslots;
	uint32_t mask;
	uint32_t num_words;		 /// The number of elements stored in the hash table.
	uint32_t num_chains;	 /// The number of non-empty chains.
	uint32_t ColissionCount; /// Number of collisions in HashTable

	uint32_t footprint;
	uint32_t compression_block_size;

	private:
		uint32_t KazLibHash(char *);
		uint32_t JZHash(char *, uint32_t);
		uint32_t djb2(char *);

	public:
		Lexicon(class InputParams *);
		~Lexicon();

		uint32_t insert(uint32_t, char *);
		uint32_t insert(char *, class InvertedList *);

		class Word * search(char *);

		void compress_index();
		void decompress_index();
		void write_index(FILE *);
		void read_index(FILE *, uint32_t);

		void display();
};

#endif // ICDS_LEXICON_H
