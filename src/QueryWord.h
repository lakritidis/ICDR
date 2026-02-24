/**
ICDR: Indexed Contrastive Data Retriever

QueryWord header file: An object used to represent a word (term or token) of a query. It extends
the Word class.

L. Akritidis, 2026
*/

#ifndef ICDR_QUERYWORD_H
#define ICDR_QUERYWORD_H

class QueryWord : public Word {
	protected:
		class InvertedListIterator *ivl_it;		/// Pointer to the term's inverted list iterator
		uint32_t q_pos;							/// The position of the term in the query

	public:
		QueryWord();
		QueryWord(class Word *, uint32_t);
		~QueryWord();

		uint32_t get_cur_docID();
		class InvertedListIterator * get_ivl_it();
		class InvertedListIterator * get_c_ivl_it() const;
};

#endif // ICDR_QUERYWORD_H
