/**
ICDR: Indexed Contrastive Data Retriever

InputData header file: An object used to store parameters and provide access to Records and Entities

L. Akritidis, 2026
*/

#ifndef ICDR_INPUTDATA_H
#define ICDR_INPUTDATA_H

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
		class Result * process_query(char *, uint32_t,
			class Lexicon *, class Entities *, class Records *, uint32_t *);

		class InputParams * get_params();
		class Records * get_records();
		class Entities * get_entities();
};


#endif // ICDR_INPUTDATA_H
