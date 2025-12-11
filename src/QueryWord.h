/// ICDR: Indexed Contrastive Data Retriever

/// Query Word Header File: An object used to represent a word (term or token) of a query. It
/// extends the Word class.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QUERYWORD_H
#define QUERYWORD_H

class QueryWord : public Word {
	protected:
		class InvertedListIterator *ivl_it;     /// Pointer to the term's inverted list iterator
		uint32_t q_pos;            /// The position of the term in the query

		score_t idf;

	public:
		QueryWord();
		QueryWord(class Word *, uint32_t);
		~QueryWord();

		void set_idf(score_t);

		uint32_t get_cur_docID();
		class InvertedListIterator * get_ivl_it();
		score_t get_idf();
};

#endif // QUERYWORD_H
