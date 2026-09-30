#include "icdr_exposed_gnu.cpp"

/// Retrieval of relevant documents
void test_retrieval(struct rettype * ret) {
	char query[1024];
	uint32_t num_results = 3;
	score_t min_thr = 0.2, max_thr = 1.0;
	struct resulttype ResultsStruct;
	class Result res;

	std::chrono::steady_clock::time_point begin, end;
	const char * test_queries[] = {
		"ninja es701uk luxe cafe pro series espresso coffee machine clearance",
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
		printf("\t%d relevant results were retrieved in %u usec\n", ResultsStruct.num_results,
				(int)std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());

		for (uint32_t j = 0; j < ResultsStruct.num_results; j++) {
			res = ResultsStruct.results[j];
			res.display();
		}
		delete [] ResultsStruct.results;
	}
}

/// Negative Sampling
void test_negatives(struct rettype * ret) {
	uint32_t num_results = 10;
	score_t min_thr = 0.0, max_thr = 1.0;
	struct resulttype ResultsStruct;
	class Result res;

	std::chrono::steady_clock::time_point begin, end;

	const uint32_t test_records[] = { 1 };

	for (uint32_t i = 0; i < sizeof(test_records) / sizeof(test_records[0]); i++) {
		class Record * rec = ret->recs->get_record(test_records[i] - 1);
		printf("\nNegative sampling for record\n");
		rec->display();
		//printf("\nThe following records must be excluded\n");
		//rec->get_matching_entity()->display();

		begin = std::chrono::steady_clock::now();
		ResultsStruct = retrieve_negative(test_records[i], 1, num_results, min_thr, max_thr, ret);
		end = std::chrono::steady_clock::now();
		printf("\t%d negative samples were retrieved in %u usec\n", ResultsStruct.num_results,
				(int)std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());

		for (uint32_t j = 0; j < ResultsStruct.num_results; j++) {
			res = ResultsStruct.results[j];
			res.display();
		}
		delete [] ResultsStruct.results;
	}
}

/// Negative Sampling
void test_all_negatives(struct rettype * ret) {
	uint32_t num_results = 10;
	score_t min_thr = 0.0, max_thr = 1.0;
	struct pairresulttype ResultsStruct;

	std::chrono::steady_clock::time_point begin, end;
	ResultsStruct = retrieve_all_negatives(1, num_results, min_thr, max_thr, ret);
}

/// PERFORM QUERY PROCESSING TESTS /////////////////////////////////////////
void perform_tests(struct rettype * ret) {
	//test_retrieval(ret);
	//test_negatives(ret);
	test_all_negatives(ret);
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

		//input_file = new char[strlen("/media/leo/7CE54B377BB9B18B/datasets/EntityResolution/ProductMatching/pricerunner/coffee_makers_2.csv") + 1];
		//strcpy(input_file, "/media/leo/7CE54B377BB9B18B/datasets/EntityResolution/ProductMatching/pricerunner/coffee_makers_2.csv");

		output_dir = new char[strlen("/home/leo/Desktop/dev/Python/FastDynamicRecordLinkage/runs/") + 1];
		strcpy(output_dir, "/home/leo/Desktop/dev/Python/FastDynamicRecordLinkage/runs/");
#elif _WIN32

		//input_file = new char[strlen("D:/datasets/EntityResolution/ProductMatching/pricerunner/coffee_makers_2.csv") + 1];
		//strcpy(input_file, "D:/datasets/EntityResolution/ProductMatching/pricerunner/coffee_makers_2.csv");
		input_file = new char[strlen("D:/datasets/EntityResolution/ProductMatching/pricerunner/tableB_large.csv") + 1];
		strcpy(input_file, "D:/datasets/EntityResolution/ProductMatching/pricerunner/tableB_large.csv");

		output_dir = new char[strlen("D:/dev/Python/FastDynamicRecordLinkage/runs/") + 1];
		strcpy(output_dir, "D:/dev/Python/FastDynamicRecordLinkage/runs/");
#endif

		/// Index construction & writing example
		// 4000037, 8388608
		ret = build(input_file, 4 * 1048576, 1, 100, 128, true, true);
 		write_index(ret, output_dir);
		//display_index(ret);
		//display_entities(ret);
		//display_records(ret);


		/// Index reading and query processing
		//ret = read_index(output_dir);
		//display_index(ret);
		//display_entities(ret);

		/// Query processing
		//ret = read_index(output_dir);
		//compute_stats(1, ret);
		perform_tests(ret);
		//retrieve_negatives(1, 10, 0.0, 1.0, ret);

		/// Deallocate Resources
		destroy(ret);
		delete [] input_file;
		delete [] output_dir;
	}

	return 0;
}
