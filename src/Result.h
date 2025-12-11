/// ICDR: Indexed Contrastive Data Retriever

/// Result Header File: An object used to represent the result object of a query.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RESULT_H
#define RESULT_H

class Result {
	uint32_t docID;
	char * text;
	score_t score;

	public:
		Result();
		Result(uint32_t, char *);
		Result(const Result&s);

		/// Manual definition of the copy assignment operator
		// Result& operator=(const Result&s) { }

		~Result();

		void destroy();

		uint32_t get_docID();
		char * get_text();
		score_t get_score();

		void set_score(double);
};

#endif // RESULT_H
