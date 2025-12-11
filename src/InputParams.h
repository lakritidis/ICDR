/// ICDR: Indexed Contrastive Data Retriever

/// InputPrams Header File: An object used to store input parameters coming from external
/// calls.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_INPUTPARAMS_H
#define ICDS_INPUTPARAMS_H

struct UserParams {
	char * input_data_file;
	char * random_string = NULL;
	char * output_dir = NULL;

	char * server_host;
	char * server_port;

	unsigned int num_req_results;
	unsigned int query_processing_algorithm;
	unsigned int index_type;
	unsigned int min_term_length;
	unsigned int max_term_length;
	unsigned int lexicon_table_size;
	unsigned int compression_block_size;
	bool handle_hyphens;
	bool handle_slashes;
	float min_sim;
	float max_sim;
};

class InputParams {
	char * input_data_file;
	char * random_string = NULL;
	char * output_dir = NULL;

	char * server_host;
	char * server_port;

	unsigned int num_req_results;
	unsigned int query_processing_algorithm;
	unsigned int index_type;
	unsigned int min_term_length;
	unsigned int max_term_length;
	unsigned int lexicon_table_size;
	unsigned int compression_block_size;
	bool handle_hyphens;
	bool handle_slashes;
	float min_sim;
	float max_sim;

	public:
		InputParams();
		explicit InputParams(struct UserParams params);
		~InputParams();

		void write(FILE *);
		void read(FILE *);

		void set_input_data_file(char *);
		void set_random_string(char *);
		void set_output_dir(char *);
		void set_server_host(char *);
		void set_server_port(char *);
		void set_num_req_results(unsigned int);
		void set_query_processing_algorithm(uint32_t);
		void set_index_type(unsigned int);
		void set_min_term_length(unsigned int);
		void set_max_term_length(unsigned int);
		void set_lexicon_table_size(unsigned int);
		void set_compression_block_size(unsigned int);
		void set_handle_hyphens(bool);
		void set_handle_slashes(bool);
		void set_min_sim(float);
		void set_max_sim(float);

		char * get_input_data_file() const;
		char * get_random_string() const;
		char * get_output_dir() const;
		char * get_server_host() const;
		char * get_server_port() const;
		unsigned int get_num_req_results() const;
		unsigned int get_query_processing_algorithm() const;
		unsigned int get_index_type() const;
		unsigned int get_min_term_length() const;
		unsigned int get_max_term_length() const;
		unsigned int get_lexicon_table_size() const;
		unsigned int get_compression_block_size() const;
		bool get_handle_hyphens() const;
		bool get_handle_slashes() const;
		float get_max_sim() const;
		float get_min_sim() const;
};


#endif //ICDS_INPUTPARAMS_H
