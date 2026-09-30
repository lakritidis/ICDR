/**
ICDR: Indexed Contrastive Data Retriever

Word header file: An object used to represent a word (term or token) of a text.

L. Akritidis, 2026
*/

#ifndef ICDR_WORD_H
#define ICDR_WORD_H

class Word {
	protected:
		uint32_t offset; /// Points at the beginning of the word in the large Lexicon buffer
		score_t idf;
		class InvertedList * ivl;

	public:
		Word();
		Word(uint32_t);
		~Word();

		void display(char *);
		void write(FILE *);

		uint32_t insert_posting(uint32_t);
		void write_list(FILE *, uint32_t);
		void read_list(FILE *);
		void compress_list(uint32_t, class Records * recs);

		void set_offset(uint32_t);
		void set_ivl(class InvertedList *);
		void set_idf(score_t);

		void get_word_string(char *, char *);
		class InvertedList * get_ivl();
		score_t get_idf();
		class Word * get_next();

		uint32_t get_offset();
		uint32_t get_freq();
		uint32_t get_footprint(uint32_t);

	friend class Lexicon;
};

#endif
