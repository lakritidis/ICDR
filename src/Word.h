/// ICDR: Indexed Contrastive Data Retriever

/// Word Header File: An object used to represent a word (term or token) of a text.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_WORD_H
#define ICDS_WORD_H

class Word {
	protected:
		char * w;
		class InvertedList * ivl;

		class Word * next;

	public:
		Word();
		Word(char *);
		virtual ~Word();

		void display();
		void write(FILE *);

		uint32_t insert_posting(uint32_t);
		void write_list(FILE *, uint32_t);
		void read_list(FILE *);
		void compress_list(uint32_t);

		void set_word_string(char *);
		void set_ivl(class InvertedList *);
		void set_next(class Word *);

		char * get_str();
		class InvertedList * get_ivl();
		class Word * get_next();

		uint32_t get_freq();

	friend class Lexicon;
};

#endif
