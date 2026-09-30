#ifndef PAIRRESULT_H
#define PAIRRESULT_H


class PairResult : public Result {
	protected:
		uint32_t left_docID;
		char *left_text;

	public:
		PairResult();
		PairResult(uint32_t, char *, uint32_t, char *);
		PairResult(const PairResult&s);

		~PairResult();

		void display();

		/// Mutators
		void set_right_text(char *);
		void set_right_docID(uint32_t);
		void set_left_text(char *);
		void set_left_docID(uint32_t);

		/// Accessors
		char * get_right_text();
		uint32_t get_right_docID();
		char * get_left_text();
		uint32_t get_left_docID();
};

#endif // PAIRRESULT_H
