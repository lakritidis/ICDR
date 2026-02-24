/**
ICDR: Indexed Contrastive Data Retriever

Result header file: An object used to represent the result object of a query.

L. Akritidis, 2026
*/

#ifndef ICDR_RESULT_H
#define ICDR_RESULT_H

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
		void display();

		uint32_t get_docID();
		char * get_text();
		score_t get_score();

		void set_score(double);
};

#endif // ICDR_RESULT_H
