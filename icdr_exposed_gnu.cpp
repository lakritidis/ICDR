/// ////////////////////////////////////////////////////////////////////////////////////////////////
/// ICDR exposed C functions - Dynamic Library References
/// C functions that i) wrap around the corresponding C++ functions and ii) are exposed to shared library.
#include "icdr.cpp"

extern "C" {

struct rettype * build(const char in[], const unsigned int lex_size, const unsigned int min_term_l,
	const unsigned int max_term_l, const unsigned int block_size, const bool hyph, const bool slas) {

		struct UserParams params{};

		params.input_data_file = (char *)malloc((strlen(in) + 1) * sizeof(char));
		strcpy(params.input_data_file, in);
		params.output_dir = NULL;
		params.random_string = NULL;
		params.lexicon_table_size = lex_size;
		params.num_req_results = 0;
		params.min_term_length = min_term_l;
		params.max_term_length = max_term_l;
		params.compression_block_size = block_size;
		params.handle_hyphens = hyph;
		params.handle_slashes = slas;
		params.min_sim = 0.0;
		params.max_sim = 0.0;

		class InputParams * PARAMS = new InputParams(params);
		InputData input_data(PARAMS);

		coding_factory cf;
		if (!PFOR_CODER) {
			PFOR_CODER = cf.get_coder(1);
		}

		if (!VBYTE_CODER) {
			VBYTE_CODER = cf.get_coder(2);
		}

		struct rettype * ret = new rettype();

		ret->lex = input_data.build_index();
		ret->ents = input_data.get_entities();
		ret->recs = input_data.get_records();
		ret->in_params = PARAMS;

		free(params.input_data_file);

		if (PFOR_CODER) {
			delete PFOR_CODER;
			PFOR_CODER = NULL;
		}

		if(VBYTE_CODER) {
			delete VBYTE_CODER;
			VBYTE_CODER = NULL;
		}

		return ret;
}

/// Process a query using the inverted index. Return results with similarity in [minsim, maxsim].

struct resulttype process_query(char * q, const unsigned int algo, const unsigned int nres,
	float mins, float maxs, struct rettype * ret) {

		struct resulttype ResultsStruct;
		uint32_t retrieved_results = 0;
		struct UserParams params{};

		params.input_data_file = NULL;
		params.output_dir = NULL;
		params.random_string = NULL;
		params.lexicon_table_size = ret->in_params->get_lexicon_table_size();
		params.compression_block_size = ret->in_params->get_compression_block_size();

		params.query_processing_algorithm = algo;
		params.num_req_results = nres;
		params.min_sim = mins;
		params.max_sim = maxs;

		class InputParams * PARAMS = new InputParams(params);
		InputData input_data(PARAMS);

		coding_factory cf;
		if (!PFOR_CODER) {
			PFOR_CODER = cf.get_coder(1);
		}

		if (!VBYTE_CODER) {
			VBYTE_CODER = cf.get_coder(2);
		}

		/// Process the Query here and "copy" the results to the output structure
		ResultsStruct.results = input_data.process_query(q, ret->lex, ret->ents, ret->recs,
			&retrieved_results);
		ResultsStruct.num_results = retrieved_results;

		delete PARAMS;

		if (PFOR_CODER) {
			delete PFOR_CODER;
			PFOR_CODER = NULL;
		}

		if(VBYTE_CODER) {
			delete VBYTE_CODER;
			VBYTE_CODER = NULL;
		}

		return ResultsStruct;
}

/// Write the inverted index and the accompanying data (Entities and Records) that are passed via
/// the ret pointer. Also write the parameters file. All four files are written in output_dir.
void write_index(struct rettype * ret, const char output_dir[]) {
	char * filename = NULL;
	uint32_t slen = strlen(output_dir) + 8;

	/// Output files
	/// prm.dat: Parameters
	filename = new char[slen + 1];
	snprintf(filename, slen + 1, "%sprm.dat", output_dir);
	filename[slen] = 0;
	FILE * prm_file = fopen(filename, "w+b");
	printf("Writing the parameters file...\n"); fflush(NULL);
	if (prm_file) {
		// printf("|%s|\n", filename);
		ret->in_params->write(prm_file);
		delete [] filename;
		fclose(prm_file);
	} else {
		printf("Error opening parameters file for writing: %s\n", filename);
		delete [] filename;
		exit(-1);
	}

	/// ivf.dat: Inverted Index
	filename = new char[slen + 1];
	snprintf(filename, slen + 1, "%sivf.dat", output_dir);
	filename[slen] = 0;
	FILE * ivf_file = fopen(filename, "w+b");
	printf("Writing the inverted file...\n"); fflush(NULL);
	if (ivf_file) {
		// printf("|%s|\n", filename);
		ret->lex->write_index(ivf_file);
		delete [] filename;
		fclose(ivf_file);
	} else {
		printf("Error opening Inverted File for writing: %s\n", filename);
		delete [] filename;
		exit(-1);
	}

	/// ent.dat: Entities
	filename = new char[slen + 1];
	snprintf(filename, slen + 1, "%sent.dat", output_dir);
	filename[slen] = 0;
	FILE * ent_file = fopen(filename, "w+b");
	printf("Writing the entities file...\n"); fflush(NULL);
	if (ent_file) {
		// printf("|%s|\n", filename);
		ret->ents->write(ent_file);
		delete [] filename;
		fclose(ent_file);
	} else {
		printf("Error opening the Entities file for writing: %s\n", filename);
		delete [] filename;
		exit(-1);
	}

	/// rec.dat: Records
	filename = new char[slen + 1];
	snprintf(filename, slen + 1, "%srec.dat", output_dir);
	filename[slen] = 0;
	FILE * rec_file = fopen(filename, "w+b");
	printf("Writing the records file...\n"); fflush(NULL);
	if (rec_file) {
		// printf("|%s|\n", filename);
		ret->recs->write(rec_file);
		delete [] filename;
		fclose(rec_file);
	} else {
		printf("Error opening the Records file for writing: %s\n", filename);
		delete [] filename;
		exit(-1);
	}

	if (PFOR_CODER) {
		delete PFOR_CODER;
		PFOR_CODER = NULL;
	}

	if(VBYTE_CODER) {
		delete VBYTE_CODER;
		VBYTE_CODER = NULL;
	}
}

/// Read the inverted index, the accompanying data (Entities and Records), and the parameters file
/// from disk and store them into a rettype structure. All four files must be in output_dir.
struct rettype * read_index(const char output_dir[]) {
	char * filename = NULL;
	uint32_t slen = strlen(output_dir) + 8;

	struct rettype * ret = new rettype();

	/// Output files
	/// prm.dat: Parameters
	filename = new char[slen + 1];
	snprintf(filename, slen + 1, "%sprm.dat", output_dir);
	filename[slen] = 0;
	FILE * prm_file = fopen(filename, "rb");
	printf("Reading the parameters file...\n"); fflush(NULL);
	if (prm_file) {
		ret->in_params = new InputParams();
		ret->in_params->read(prm_file);
		delete [] filename;
		fclose(prm_file);
	} else {
		printf("Error opening the parameters file for reading: %s\n", filename);
		delete [] filename;
		exit(-1);
	}

	/// ivf.dat: Inverted Index
	filename = new char[slen + 1];
	snprintf(filename, slen + 1, "%sivf.dat", output_dir);
	filename[slen] = 0;
	FILE * ivf_file = fopen(filename, "rb");
	printf("Reading the inverted file...\n"); fflush(NULL);
	if (ivf_file) {
		ret->lex = new Lexicon(ret->in_params);
		ret->lex->read_index(ivf_file, ret->in_params->get_compression_block_size());
		delete [] filename;
		fclose(ivf_file);
	} else {
		printf("Error opening Inverted File for reading: %s\n", filename);
		delete [] filename;
		exit(-1);
	}

	/// ent.dat: Entities
	filename = new char[slen + 1];
	snprintf(filename, slen + 1, "%sent.dat", output_dir);
	filename[slen] = 0;
	FILE * ent_file = fopen(filename, "rb");
	printf("Reading the entities file...\n"); fflush(NULL);
	if (ent_file) {
		ret->ents = new Entities(1048576);
		ret->ents->read(ent_file);
		delete [] filename;
		fclose(ent_file);
	} else {
		printf("Error opening the Entities file for reading: %s\n", filename);
		delete [] filename;
		exit(-1);
	}

	/// rec.dat: Records
	filename = new char[slen + 1];
	snprintf(filename, slen + 1, "%srec.dat", output_dir);
	filename[slen] = 0;
	FILE * rec_file = fopen(filename, "rb");
	printf("Reading the records file...\n"); fflush(NULL);
	if (rec_file) {
		ret->recs = new Records();
		ret->recs->read(rec_file);
		delete [] filename;
		fclose(rec_file);
	} else {
		printf("Error opening the Records file for reading: %s\n", filename);
		delete [] filename;
		exit(-1);
	}

	if (PFOR_CODER) {
		delete PFOR_CODER;
		PFOR_CODER = NULL;
	}

	if(VBYTE_CODER) {
		delete VBYTE_CODER;
		VBYTE_CODER = NULL;
	}

	return ret;
}

/// Destroy and deallocate the memory occupied by the three primary data structures:
/// The inverted index, the entities object and the array or Records
void destroy(struct rettype * ret) {
	delete ret->lex;
	delete ret->ents;
	delete ret->recs;

	if (PFOR_CODER) {
		delete PFOR_CODER;
		PFOR_CODER = NULL;
	}

	if(VBYTE_CODER) {
		delete VBYTE_CODER;
		VBYTE_CODER = NULL;
	}

	delete ret->in_params;
	delete ret;
}

/// Display the inverted index
void display_index(struct rettype * ret) {
	coding_factory cf;
	if (!PFOR_CODER) {
		PFOR_CODER = cf.get_coder(1);
	}

	if (!VBYTE_CODER) {
		VBYTE_CODER = cf.get_coder(2);
	}

	ret->lex->display();

	if (PFOR_CODER) {
		delete PFOR_CODER;
		PFOR_CODER = NULL;
	}

	if(VBYTE_CODER) {
		delete VBYTE_CODER;
		VBYTE_CODER = NULL;
	}
}

/// Display the Entities
void display_entities(struct rettype * ret) {
	ret->ents->display();
}

}
