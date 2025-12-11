/// ICDR: Indexed Contrastive Data Retriever

/// InputData Implementation File: An object used to store parameters, and provide access to the
/// Records and the Entities
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_INPUTDATA_CPP
#define ICDS_INPUTDATA_CPP

#include "InputData.h"


/// Constructor
InputData::InputData(class InputParams * pr) :
		params(pr),
		records(NULL),
		entities(NULL) {
}

/// Destructor
InputData::~InputData() {
}


/// Construct the inverted index
class Lexicon * InputData::build_index() {
	char * input_data;
	long file_size = 0;

	FILE * datafile = fopen(this->params->get_input_data_file(), "r");
	class Lexicon * lex = NULL;
	if (datafile) {
		input_data = this->read_file(datafile, &file_size);
		this->process_data(input_data, file_size);

		// this->entities->display();
		lex = this->construct_index();

		free(input_data);
		fclose(datafile);
	} else {
		printf("Error Opening Input File %s\n", this->params->get_input_data_file());
		exit(0);
	}
	return lex;
}

/// Build an inverted index structure
class Lexicon * InputData::construct_index() {
	char * text;
	char word[128];

	uint32_t res = 0;
	uint32_t r = 0, pos = 0, text_length = 0, y = 0, rid = 0;
	uint32_t num_words = 0, num_uwords = 0, total_num_words = 0;

	class Lexicon * lex = new Lexicon(this->params);
	class Record * rec = NULL;

	for (r = 0; r < this->records->get_num_records(); r++) {
		rec = this->records->get_record(r);

		rid = rec->get_id();
		text = rec->get_text();
		text_length = strlen(text);
		// printf("Record Text: %s\n", text);

		num_words = 0;
		num_uwords = 0;

		for (pos = 0; pos < text_length; pos++) {
			if (text[pos] == 32) {
				word[y] = 0;
				if (y > this->params->get_min_term_length()) {
					res = lex->insert(rid, word);
					if (res > 0) {
						num_words += 1;
						total_num_words += 1;
					}
					if (res == 1) {
						num_uwords += 1;
					}
					//printf("\tWord: %s (%d, %d, %d)\n", word, num_words, num_uwords, total_num_words);
				}
				y = 0;
				pos++;
			}
			word[y++] = text[pos];
		}

		word[y] = 0;
		if (y > this->params->get_min_term_length()) {
			res = lex->insert(rid, word);
			if (res > 0) {
				num_words += 1;
				total_num_words += 1;
			}
			if (res == 1) {
				num_uwords += 1;
			}
			//printf("\tWord: %s (%d, %d, %d)\n", word, num_words, num_uwords, total_num_words);
		}
		y = 0;

		rec->set_word_len(num_words);
		rec->set_uword_len(num_uwords);
	}

	lex->compress_index();

	this->records->set_total_num_words( total_num_words );

	/// Commenting that creates a memory leak.
	/// However, it also allows us to output the Lexicon to the Python lib. The user is responsible
	/// for dealloacting that via a manual invocation of "destroy".
	// delete lex;
	return lex;
}

void InputData::process_data(char * input_data, uint32_t len) {
	class Entity * ent = NULL;
	uint32_t i = 0, occ = 0, y = 0, num_records = 0;
	char buf[16384], record_title[16384], entity_code[16384];

	this->records = new Records(128);
	this->entities = new Entities(1048576);

	for (i = 0; i < len; i++) {
		/// A comma character was found
		if (input_data[i] == 44) {
			if (occ == 0) {
				buf[y] = 0;
				strcpy(record_title, buf);

				occ = 1;
				y = 0;
			}
		/// A line-break character was found
		} else if (input_data[i] == 10) {
			buf[y] = 0;
			strcpy(entity_code, buf);
			occ = 0;
			y = 0;

			ent = this->entities->insert(entity_code, num_records + 1);
			this->records->insert(++num_records, record_title, ent);

		/// Otherwise (neither comma, nor line-break characters), we copy the data to the buffer.
		} else {
			buf[y++] = input_data[i];
		}
	}
}

/// Read the contents of an input data file into a byte buffer.
char * InputData::read_file(FILE * source, long * file_size) {
	fseek(source, 0L, SEEK_END);
	*file_size = ftell(source);
	rewind(source);

	char * out = (char *)malloc((*file_size + 1) * sizeof(char));

	int nread = 0, c;
	while ((c = fgetc(source)) != EOF) {
		out[nread] = c;
		nread++;
	}
	out[nread - 1] = 0;
	*file_size = nread;

	// printf(out);
	return out;
}

/// Process an input query and return an array Result objects
class Result * InputData::process_query(char * q_str, class Lexicon * lex, class Entities * ents,
	class Records * records, uint32_t * retrieved_results) {
		class Result * results = NULL;

		this->entities = ents;
		this->records = records;

		class Query * q = new Query(q_str, this->params, lex, this->records);
		if (this->params->get_query_processing_algorithm() == 2) {
		} else {
			results = q->process_DAAT();
		}

		*retrieved_results = q->get_num_results();

		delete q;

		return results;
}

inline class InputParams * InputData::get_params() { return this->params; }
inline class Records * InputData::get_records() { return this->records; }
inline class Entities * InputData::get_entities() { return this->entities; }

#endif
