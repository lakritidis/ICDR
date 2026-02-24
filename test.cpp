#include "icdr_exposed_gnu.cpp"

void test_fst() {
    FST *fst = fst_create();

    fst_insert(fst, "cat", 1);
    fst_insert(fst, "car", 2);
    fst_insert(fst, "dog", 3);
    fst_insert(fst, "door", 4);

    uint32_t value;

    if (fst_lookup(fst, "cat", &value))
        printf("Found cat: %u\n", value);

    if (fst_lookup(fst, "dog", &value))
        printf("Found dog: %u\n", value);

    printf("\nAll entries:\n");
    fst_print_all(fst);

    fst_free(fst);
}

void test_fst_packed() {
/*
    FST *fst = build_example();

    uint32_t value;

    if (fst_lookup(fst, "cat", &value))
        printf("cat -> %u\n", value);

    if (fst_lookup(fst, "car", &value))
        printf("car -> %u\n", value);

    if (fst_lookup(fst, "dog", &value))
        printf("dog -> %u\n", value);

    if (!fst_lookup(fst, "cow", &value))
        printf("cow not found\n");

    fst_free(fst);
*/
}

/// PERFORM QUERY PROCESSING TESTS /////////////////////////////////////////
void perform_tests(struct rettype * ret) {
	char query[1024];
	uint32_t num_results = 100;
	score_t min_thr = 0.2, max_thr = 1.0;
	struct resulttype ResultsStruct;
	class Result res;

	std::chrono::steady_clock::time_point begin, end;

	/// Retrieval of relevant documents
	const char * test_queries[] = {
		"bosch coffee maker",
		"bosch coffee machine",
		"whirlpool coffee machine"
	};

	for (uint32_t i = 0; i < sizeof(test_queries) / sizeof(test_queries[0]); i++) {
		strcpy(query, test_queries[i]);
		printf("\nProcessing the query '%s'...\n", query);
		begin = std::chrono::steady_clock::now();
		ResultsStruct = retrieve_relevant(query, 1, num_results,  min_thr, max_thr, ret);
		end = std::chrono::steady_clock::now();
		printf("\t%d relevant results were retrieved in %llu usec\n", ResultsStruct.num_results,
				std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());

		for (uint32_t j = 0; j < ResultsStruct.num_results; j++) {
			res = ResultsStruct.results[j];
			//res.display();
		}
		delete [] ResultsStruct.results;
	}

	/// Negative Sampling
	const uint32_t test_records[] = { 1000 };

	for (uint32_t i = 0; i < sizeof(test_records) / sizeof(test_records[0]); i++) {
		class Record * rec = ret->recs->get_record(test_records[i] - 1);
		printf("\nNegative sampling for record\n");
		rec->display();
		//printf("\nThe following records must be excluded\n");
		//rec->get_matching_entity()->display();

		begin = std::chrono::steady_clock::now();
		ResultsStruct = retrieve_negative(test_records[i], 1, num_results, min_thr, max_thr, ret);
		end = std::chrono::steady_clock::now();
		printf("\t%d negative samples were retrieved in %llu usec\n", ResultsStruct.num_results,
				std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());

		for (uint32_t j = 0; j < ResultsStruct.num_results; j++) {
			res = ResultsStruct.results[j];
			// res.display();
		}
		delete [] ResultsStruct.results;
	}
}



/// MAIN /////////////////////////////////////////
int main(int argc, char *argv[]) {
	char * input_file = NULL, *output_dir = NULL;
	struct rettype * ret;

	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);

	if (argc == 1) {
#ifdef __linux__
		input_file = new char[strlen("/media/leo/7CE54B377BB9B18B/datasets/EntityResolution/ProductMatching/pricerunner/tableB_large.csv") + 1];
		strcpy(input_file, "/media/leo/7CE54B377BB9B18B/datasets/EntityResolution/ProductMatching/pricerunner/tableB_large.csv");

		output_dir = new char[strlen("/home/leo/Desktop/dev/Python/FastDynamicRecordLinkage/runs/") + 1];
		strcpy(output_dir, "/home/leo/Desktop/dev/Python/FastDynamicRecordLinkage/runs/");
#elif _WIN32

		input_file = new char[strlen("D:/datasets/EntityResolution/ProductMatching/pricerunner/tableB_large.csv") + 1];
		strcpy(input_file, "D:/datasets/EntityResolution/ProductMatching/pricerunner/tableB_large.csv");

		output_dir = new char[strlen("D:/dev/Python/FastDynamicRecordLinkage/runs/") + 1];
		strcpy(output_dir, "D:/dev/Python/FastDynamicRecordLinkage/runs/");
/*
		input_file = new char[strlen("C:/Users/Owner/Documents/cpp/coffee_makers_2.csv") + 1];
		strcpy(input_file, "C:/Users/Owner/Documents/cpp/coffee_makers_2.csv");

		output_dir = new char[strlen("C:/Users/Owner/Documents/cpp/icds_data/") + 1];
		strcpy(output_dir, "C:/Users/Owner/Documents/cpp/icds_data/");
*/
#endif
		/// Index construction & writing example
		// 4000037
		ret = build(input_file, 2097152, 1, 100, 128, true, true);
 		// write_index(ret, output_dir);
//		display_index(ret);
//		display_entities(ret);

/*
		/// Index reading and query processing
		ret = read_index(output_dir);
		display_index(ret);
		display_entities(ret);
*/

		/// Query processing
		// ret = read_index(output_dir);
		// compute_stats(1, ret);
		perform_tests(ret);

		/// Deallocate Resources
		destroy(ret);
		delete [] input_file;
		delete [] output_dir;
	}

	return 0;
}
