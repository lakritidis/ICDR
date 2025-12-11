/// ICDR: Indexed Contrastive Data Retriever

/// InputData Header File: An object used to store parameters, and provide access to the
/// Records and the Entities
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_INPUTDATA_H
#define ICDS_INPUTDATA_H

class InputData {
	InputParams * params;
	class Records * records;
	class Entities * entities;

	private:
		char *read_file(FILE *, long *);
		void process_data(char *, uint32_t);
		class Lexicon * construct_index();

	public:
		InputData(class InputParams * pr);
		~InputData();

		class Lexicon * build_index();
		class Result * process_query(char *, class Lexicon *, class Entities *, class Records *, uint32_t *);

		class InputParams * get_params();
		class Records * get_records();
		class Entities * get_entities();
};


#endif // ICDS_INPUTDATA_H
