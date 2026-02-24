/**
ICDR: Indexed Contrastive Data Retriever

Word header file: An object used to represent a word (term or token) of a text.

L. Akritidis, 2026
*/

#ifndef ICDR_WORD_H
#define ICDR_WORD_H

class Word {
	protected:
		char * w;
		class InvertedList * ivl;
		score_t idf;

		class Word * next;

	public:
		Word();
		Word(char *);
		~Word();

		void display();
		void write(FILE *);

		uint32_t insert_posting(uint32_t);
		void write_list(FILE *, uint32_t);
		void read_list(FILE *);
		void compress_list(uint32_t, class Records * recs);

		void set_word_string(char *);
		void set_ivl(class InvertedList *);
		void set_idf(score_t);
		void set_next(class Word *);

		char * get_str();
		class InvertedList * get_ivl();
		score_t get_idf();
		class Word * get_next();

		uint32_t get_freq();
		uint32_t get_footprint(uint32_t);

	friend class Lexicon;
};

#endif
