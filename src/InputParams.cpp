/// ICDR: Indexed Contrastive Data Retriever

/// InputPrams Implementation File: An object used to store input parameters coming from external
/// calls.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////


#ifndef ICDS_INPUTPARAMS_CPP
#define ICDS_INPUTPARAMS_CPP

#include "InputParams.h"

/// Default constructor without arguments (used when the execution parameters are read from disk).
InputParams::InputParams() :
	input_data_file(NULL),
	random_string(NULL),
	output_dir(NULL),
	server_host(NULL),
	server_port(NULL),
	num_req_results(0),
	query_processing_algorithm(0),
	index_type(0),
	min_term_length(0),
	max_term_length(0),
	lexicon_table_size(0),
	compression_block_size(0),
	handle_hyphens(false),
	handle_slashes(false),
	min_sim(0.0),
	max_sim(0.0) {
}

/// Constructor that copies the data from a UserParams structure to a new object
InputParams::InputParams(struct UserParams uparams) :
	input_data_file(NULL),
	random_string(NULL),
	output_dir(NULL),
	server_host(NULL),
	server_port(NULL),
	num_req_results(uparams.num_req_results),
	query_processing_algorithm(uparams.query_processing_algorithm),
	index_type(uparams.index_type),
	min_term_length(uparams.min_term_length),
	max_term_length(uparams.max_term_length),
	lexicon_table_size (uparams.lexicon_table_size),
	compression_block_size(uparams.compression_block_size),
	handle_hyphens(uparams.handle_hyphens),
	handle_slashes(uparams.handle_slashes),
	min_sim(uparams.min_sim),
	max_sim(uparams.max_sim) {

		if (uparams.input_data_file) {
			this->set_input_data_file(uparams.input_data_file);
		}

		if (uparams.random_string) {
			this->set_random_string(uparams.random_string);
		}

		if (uparams.output_dir) {
			this->set_output_dir(uparams.output_dir);
		}

		if (uparams.server_host) {
			this->set_server_host(uparams.server_host);
		}

		if (uparams.server_port) {
			this->set_server_port(uparams.server_port);
		}
}

/// Destructor
InputParams::~InputParams() {
	if (this->input_data_file) {
		delete [] this->input_data_file;
	}

	if (this->random_string) {
		delete [] this->random_string;
	}

	if (this->output_dir) {
		delete [] this->output_dir;
	}

	if (this->server_host) {
		delete [] this->server_host;
	}

	if (this->server_port) {
		delete [] this->server_port;
	}
}

/// Write the execution parameters to disk
void InputParams::write(FILE * fp) {
	uint32_t l = 0;
	if (fp) {
		if (this->input_data_file) {
			l = strlen(this->input_data_file);
			fwrite(&l, sizeof(uint32_t), 1, fp);
			fwrite(this->input_data_file, sizeof(char), l, fp);
		} else {
			l = 0;
			fwrite(&l, sizeof(uint32_t), 1, fp);
		}

		if (this->random_string) {
			l = strlen(this->random_string);
			fwrite(&l, sizeof(uint32_t), 1, fp);
			fwrite(this->random_string, sizeof(char), l, fp);
		} else {
			l = 0;
			fwrite(&l, sizeof(uint32_t), 1, fp);
		}

		if (this->output_dir) {
			l = strlen(this->output_dir);
			fwrite(&l, sizeof(uint32_t), 1, fp);
			fwrite(this->output_dir, sizeof(char), l, fp);
		} else {
			l = 0;
			fwrite(&l, sizeof(uint32_t), 1, fp);
		}

		fwrite(&this->num_req_results, sizeof(uint32_t), 1, fp);
		fwrite(&this->index_type, sizeof(uint32_t), 1, fp);
		fwrite(&this->min_term_length, sizeof(uint32_t), 1, fp);
		fwrite(&this->max_term_length, sizeof(uint32_t), 1, fp);
		fwrite(&this->lexicon_table_size, sizeof(uint32_t), 1, fp);
		fwrite(&this->compression_block_size, sizeof(uint32_t), 1, fp);
		fwrite(&this->query_processing_algorithm, sizeof(uint32_t), 1, fp);
		fwrite(&this->index_type, sizeof(uint32_t), 1, fp);
		fwrite(&this->handle_hyphens, sizeof(bool), 1, fp);
		fwrite(&this->handle_slashes, sizeof(bool), 1, fp);
		fwrite(&this->min_sim, sizeof(float), 1, fp);
		fwrite(&this->max_sim, sizeof(float), 1, fp);
	} else {
		printf("Error opening the data file for input parameters\n");
		exit(-1);
	}
}

/// Read the execution parameters from disk (written from a previous index construction)
void InputParams::read(FILE * fp) {
	uint32_t v = 0;

	if (fp) {
		fread(&v, sizeof(uint32_t), 1, fp);
		if (v > 0) {
			this->input_data_file = new char[v + 1];
			fread(this->input_data_file, sizeof(char), v, fp);
			this->input_data_file[v] = 0;
		} else {
			this->input_data_file = NULL;
		}

		fread(&v, sizeof(uint32_t), 1, fp);
		if (v > 0) {
			this->random_string = new char[v + 1];
			fread(this->random_string, sizeof(char), v, fp);
			this->random_string[v] = 0;
		} else {
			this->random_string = NULL;
		}

		fread(&v, sizeof(uint32_t), 1, fp);
		if (v > 0) {
			this->output_dir = new char[v + 1];
			fread(this->output_dir, sizeof(char), v, fp);
			this->output_dir[v] = 0;
		} else {
			this->output_dir = NULL;
		}

		fread(&this->num_req_results, sizeof(uint32_t), 1, fp);
		fread(&this->index_type, sizeof(uint32_t), 1, fp);
		fread(&this->min_term_length, sizeof(uint32_t), 1, fp);
		fread(&this->max_term_length, sizeof(uint32_t), 1, fp);
		fread(&this->lexicon_table_size, sizeof(uint32_t), 1, fp);
		fread(&this->compression_block_size, sizeof(uint32_t), 1, fp);
		fread(&this->query_processing_algorithm, sizeof(uint32_t), 1, fp);
		fread(&this->index_type, sizeof(uint32_t), 1, fp);
		fread(&this->handle_hyphens, sizeof(bool), 1, fp);
		fread(&this->handle_slashes, sizeof(bool), 1, fp);
		fread(&this->min_sim, sizeof(float), 1, fp);
		fread(&this->max_sim, sizeof(float), 1, fp);

	} else {
		printf("Error opening the data file for input parameters\n");
		exit(-1);
	}
}

/// Mutators
void InputParams::set_input_data_file(char * v) {
	this->input_data_file = new char[strlen(v) + 1];
	strcpy(this->input_data_file, v);
}

void InputParams::set_random_string(char * v) {
	this->random_string = new char[strlen(v) + 1];
	strcpy(this->random_string, v);
}

void InputParams::set_output_dir(char * v) {
	this->output_dir = new char[strlen(v) + 1];
	strcpy(this->output_dir, v);
}

void InputParams::set_server_host(char * v) {
	this->server_host = new char[strlen(v) + 1];
	strcpy(this->server_host, v);
}

void InputParams::set_server_port(char * v) {
	this->server_port = new char[strlen(v) + 1];
	strcpy(this->server_port, v);
}

void InputParams::set_num_req_results(unsigned int v) { this->num_req_results = v; }
void InputParams::set_min_term_length(unsigned int v) { this->min_term_length = v; }
void InputParams::set_max_term_length(unsigned int v) { this->max_term_length = v; }
void InputParams::set_lexicon_table_size(unsigned int v) { this->lexicon_table_size = v; }
void InputParams::set_compression_block_size(unsigned int v) { this->compression_block_size = v; }
void InputParams::set_index_type(unsigned int v) { this->index_type = v; }
void InputParams::set_query_processing_algorithm(unsigned int v) { this->query_processing_algorithm = v; }
void InputParams::set_handle_hyphens(bool v) { this->handle_hyphens = v; }
void InputParams::set_handle_slashes(bool v) { this->handle_slashes = v; }
void InputParams::set_min_sim(float v) { this->min_sim = v; }
void InputParams::set_max_sim(float v) { this->max_sim = v; }

/// Accessors
inline char * InputParams::get_input_data_file() const { return this->input_data_file; }
inline char * InputParams::get_random_string() const { return this->random_string; }
inline char * InputParams::get_output_dir() const { return this->output_dir; }
inline char * InputParams::get_server_host() const { return this->server_host; }
inline char * InputParams::get_server_port() const { return this->server_port; }
inline unsigned int InputParams::get_num_req_results() const { return this->num_req_results; }
inline unsigned int InputParams::get_index_type() const { return this->index_type; }
inline unsigned int InputParams::get_query_processing_algorithm() const { return this->query_processing_algorithm; }
inline unsigned int InputParams::get_min_term_length() const { return this->min_term_length; }
inline unsigned int InputParams::get_max_term_length() const { return this->max_term_length; }
inline unsigned int InputParams::get_lexicon_table_size() const { return this->lexicon_table_size; }
inline unsigned int InputParams::get_compression_block_size() const { return this->compression_block_size; }
inline bool InputParams::get_handle_hyphens() const { return this->handle_hyphens; }
inline bool InputParams::get_handle_slashes() const { return this->handle_slashes; }
inline float InputParams::get_min_sim() const { return this->min_sim; }
inline float InputParams::get_max_sim() const { return this->max_sim; }

#endif
