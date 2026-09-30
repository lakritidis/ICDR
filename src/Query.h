/**
ICDR: Indexed Contrastive Data Retriever

Query header file: An object used to represent an input query. Implements various
query processing algorithms.

L. Akritidis, 2026
*/

#ifndef ICDR_QUERY_H
#define ICDR_QUERY_H

template<class T> class Query {
	char * query_string;		/// The query string as entered by the user
	uint32_t num_terms;		/// The number of Query terms
	std::vector<QueryWord *> query_terms;
	uint32_t num_results;
	class Lexicon * lexicon;
	class InputParams * qparams;
	class Records * doc_info;
	char tmp[MAX_TERM_LENGTH];

	private:
		uint32_t insert_term(char *, uint32_t);
		static int32_t compare_qterms(const void *, const void *);

		uint32_t max_docID();
		uint32_t min_docID();
		bool lists_exhausted();
		void score_BM25(uint32_t, class MaxHeap<T> *, uint32_t);

		void extract_query_terms(int32_t);
		void display_query_terms();

	public:
		Query();
		Query(char *, class InputParams *, class Lexicon *, class Records *);
		~Query();

		void process(uint32_t, uint32_t, T *, uint32_t);
		void process_DAAT(uint32_t, T *, uint32_t);
		void process_BMW(uint32_t, T *, uint32_t);
		uint32_t get_num_results();

		char * get_query_string();

		void set_query_string(char *);
		void set_query_params(class InputParams *);
		void set_doc_info(class Records *);
		void set_lexicon(class Lexicon *);
};

#endif
