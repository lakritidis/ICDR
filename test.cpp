#include "icdr_exposed_gnu.cpp"

int main(int argc, char *argv[]) {
	char * input_file = NULL, *output_dir = NULL;
	struct rettype * ret;


	if (argc == 1) {
#ifdef __linux__
		input_file = new char[strlen("/media/leo/7CE54B377BB9B18B/datasets/EntityResolution/ProductMatching/pricerunner/coffee_makers_2.csv") + 1];
		strcpy(input_file, "/media/leo/7CE54B377BB9B18B/datasets/EntityResolution/ProductMatching/pricerunner/coffee_makers_2.csv");

		output_dir = new char[strlen("/home/leo/Desktop/dev/Python/FastDynamicRecordLinkage/runs/") + 1];
		strcpy(output_dir, "/home/leo/Desktop/dev/Python/FastDynamicRecordLinkage/runs/");
#elif _WIN32
		input_file = new char[strlen("D:/datasets/EntityResolution/ProductMatching/pricerunner/coffee_makers_2.csv") + 1];
		strcpy(input_file, "D:/datasets/EntityResolution/ProductMatching/pricerunner/coffee_makers_2.csv");

		output_dir = new char[strlen("D:/dev/Python/FastDynamicRecordLinkage/runs/") + 1];
		strcpy(output_dir, "D:/dev/Python/FastDynamicRecordLinkage/runs/");
#endif

/*
		/// Index construction & writing example
		ret = build(input_file, 300007, 1, 100, 128, true, true);
		write_index(ret, output_dir);
		display_index(ret);
		display_entities(ret);
*/

/*
		/// Index reading and query processing
		ret = read_index(output_dir);
		display_index(ret);
		display_entities(ret);
*/


		/// Query processing
		ret = read_index(output_dir);

		char query[1024];
		uint32_t num_results = 1000;
		struct resulttype ResultsStruct;
		class Result res;

		strcpy(query, "bosch coffee maker");
		ResultsStruct = process_query(query, 1, num_results,  0.60, 0.70, ret);
		printf("\n%d relevant results were retrieved for '%s'\n", ResultsStruct.num_results, query);
		for (uint32_t i = 0; i < ResultsStruct.num_results; i++) {
			res = ResultsStruct.results[i];
			printf("\tTitle: %s, Score: %5.3f\n", res.get_text(), res.get_score());
		}
		delete [] ResultsStruct.results;


		strcpy(query, "bosch coffee machine");
		ResultsStruct = process_query(query, 1, num_results,  0.60, 0.70, ret);
		printf("\n%d relevant results were retrieved for '%s'\n", ResultsStruct.num_results, query);
		for (uint32_t i = 0; i < ResultsStruct.num_results; i++) {
			res = ResultsStruct.results[i];
			printf("\tTitle: %s, Score: %5.3f\n", res.get_text(), res.get_score());
		}
		delete [] ResultsStruct.results;


		/// Deallocate Resources
		destroy(ret);
		delete [] input_file;
		delete [] output_dir;
	}

	return 0;
}
