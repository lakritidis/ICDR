/// ICDR: Indexed Contrastive Data Retriever

/// Query Header File: An object used to represent a query.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QUERY_H
#define QUERY_H

class Query {
	char *query_string;		/// The query string as entered by the user
	uint32_t num_terms;		/// The number of Query terms
	class QueryWord ** query_terms;
	uint32_t num_results;
	class InputParams * qparams;
	class Records * doc_info;

	private:
		uint32_t insert_term(char *, uint32_t, class Lexicon *);
		static int32_t compare(const void *, const void *);

		uint32_t max_docID();
		uint32_t min_docID();
		bool lists_exhausted();
		void score_BM25(uint32_t, class MaxHeap *);
		void exhaustiveEvaluation(class BinaryHeap *);

	public:
		Query();
		Query(char *, class InputParams *, class Lexicon *, class Records *);
		~Query();

		class Result * process_DAAT();
		uint32_t get_num_results();
};

#endif
