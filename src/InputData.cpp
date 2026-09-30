/**
ICDR: Indexed Contrastive Data Retriever

InputData implementation file: An object used to store parameters and provide access to Records
and Entities.

L. Akritidis, 2026
*/

#ifndef ICDR_INPUTDATA_CPP
#define ICDR_INPUTDATA_CPP

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

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	FILE * datafile = fopen(this->params->get_input_data_file(), "r");
	class Lexicon * lex = NULL;
	if (datafile) {
		input_data = this->read_file(datafile, &file_size);
		this->process_data(input_data, file_size);

		lex = this->construct_index();

		free(input_data);
		fclose(datafile);
	} else {
		fprintf(stderr, "Error Opening Input File %s\n", this->params->get_input_data_file());
		exit(-1);
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	printf("Index construction completed in %5.3f sec.\n",
		std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0f);

	return lex;
}

/// Build an inverted index structure
class Lexicon * InputData::construct_index() {
	char * text;
	char word[MAX_TERM_LENGTH], temp_word[MAX_TERM_LENGTH];

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
					res = lex->insert(rid, word, temp_word);
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
			res = lex->insert(rid, word, temp_word);
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

	this->records->set_total_num_words( total_num_words );

	lex->compress_index(this->records);

	/// Commenting that creates a memory leak.
	/// However, it also allows us to output the Lexicon to the Python lib. The user is responsible
	/// for dealloacting that via a manual invocation of "destroy".
	// delete lex;
	// lex->display_hash_table_performance();
	return lex;
}

void InputData::process_data(char * input_data, uint32_t len) {
	class Entity * ent = NULL;
	uint32_t i = 0, occ = 0, y = 0, num_records = 0;
	char buf[16384], record_title[16384], entity_code[16384];

	this->records = new Records(128);
	this->entities = new Entities(1000003);

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

/// Process an input query and return an array Result objects. If rec_id = 0 then, process the
/// query q like a typical search engine. If rec_id > 0, make q = rec_title and retrieve samples
/// that are negative to rec_id.
class Result * InputData::process_query(char * q_str, uint32_t rec_id, class Lexicon * lex,
	uint32_t * retrieved_results) {
		class Result * results = new Result[this->params->get_num_req_results()];

		std::chrono::steady_clock::time_point begin, end;
		/// For negative sampling, make the query string equal to the text of the Record for
		/// which we are performing negative sampling. Otherwise, q is user-defined.
		if (rec_id > 0) {
			class Record * Rec = records->get_record(rec_id - 1);
			q_str = new char[strlen(Rec->get_text()) + 1];
			strcpy(q_str, Rec->get_text());
		}

		class Query<Result> * q = new Query<Result>(q_str, this->params, lex, this->records);
		begin = std::chrono::steady_clock::now();
		q->process(this->params->get_query_processing_algorithm(), rec_id, results, 0);
		end = std::chrono::steady_clock::now();
		// printf("\tDURATION: %llu usec\n", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
		*retrieved_results = q->get_num_results();

		delete q;
		if (rec_id > 0) {
			delete [] q_str;
		}
		return results;
}

/// Retrieve a negative samples for each record in the Records array.
class PairResult * InputData::retrieve_all_negatives(class Lexicon * lex, uint32_t * retrieved_results) {
	uint32_t i = 0, start = 0;
	class Record * r = NULL;
	std::chrono::steady_clock::time_point begin, end;

	/// Preallocate space t store the results for multiple queries: num_queries * num_results
	uint32_t num_alloc_results = this->records->get_num_records() * this->params->get_num_req_results();

	class PairResult * results = new PairResult[num_alloc_results];
	// printf("Allocated space for %d results\n", num_alloc_results);

	class Query<PairResult> * q = new Query<PairResult>();
	q->set_query_params(this->params);
	q->set_doc_info(this->records);
	q->set_lexicon(lex);

	begin = std::chrono::steady_clock::now();
	for (i = 0; i < this->records->get_num_records(); i++) {
		r = this->records->get_record(i);
		if (r) {
			q->set_query_string(r->get_text());
			q->process(this->params->get_query_processing_algorithm(), r->get_id(), results, start);
/*
			printf("Record ID %d, Title: %s\n", r->get_id(), q->get_query_string());
			for (uint32_t j = start; j < start + q->get_num_results(); j++) {
				printf("\tNegative %d: %d - %s, %d - %s --- Score: %5.3f\n", j,
					results[j].get_left_docID(), results[j].get_left_text(),
					results[j].get_right_docID(), results[j].get_right_text(), results[j].get_score());
			}
			getchar();
*/
			start += q->get_num_results();
			//printf("\tStart: %d\n", start);
		}

		/// This avoids double deletion, since the query string is the entity title.
		q->set_query_string(NULL);

	}
	end = std::chrono::steady_clock::now();

	delete q;

	return results;
}

inline class InputParams * InputData::get_params() { return this->params; }
inline class Records * InputData::get_records() { return this->records; }
inline class Entities * InputData::get_entities() { return this->entities; }

inline void InputData::set_params(class InputParams * v) { this->params = v; }
inline void InputData::set_records(class Records * v) { this->records = v; }
inline void InputData::set_entities(class Entities * v) { this->entities = v; }
#endif
