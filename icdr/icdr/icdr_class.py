import ctypes
import pandas as pd

# For Deployment
#from icdr.icdr_base import icdr_base
#from icdr.icdr_types import IndexDataConnector
#from icdr.icdr_types import ResultsTypeConnector, ResultTypeConnector, PairResultsTypeConnector, PairResultTypeConnector
#from icdr.icdr_types import RecordsTypeConnector, RecordTypeConnector, EntitiesTypeConnector, EntityTypeConnector


# For development
from icdr_base import icdr_base
from icdr_types import IndexDataConnector
from icdr_types import ResultsTypeConnector, ResultTypeConnector, PairResultTypeConnector, PairResultsTypeConnector
from icdr_types import RecordsTypeConnector, RecordTypeConnector, EntitiesTypeConnector, EntityTypeConnector

class icdr(icdr_base):
    def __init__(self):
        """
        Index-Assisted Contrastive Data Synthesizer

        Declare the input and the output of the library's exposed functions.
        The dynamic library exposes the following functions:
        `build`, `query`, `write`, `read`, `destroy`, `display_index` and `display_entities`.
        """
        icdr_base.__init__(self)

        self.output_dir = None
        self.index_data = None

        # Build index
        # const char in[], const unsigned int lex_size, const unsigned int min_term_l, const unsigned int block_size,
        # const bool hyph, const bool slas
        self.icdr_lib.build.argtypes = [
            ctypes.c_char_p,        # Input data file with the text collection to be indexed
            ctypes.c_int,           # Lexicon hash table size (int)
            ctypes.c_int,           # Minimum term length to be indexed (int)
            ctypes.c_int,           # Maximum term length to be indexed (int)
            ctypes.c_int,           # Index compression block size (int)
            ctypes.c_bool,          # Handle hyphens (bool)
            ctypes.c_bool           # Handle slashes (bool)
        ]
        self.icdr_lib.build.restype = IndexDataConnector

        # Submit a query to the inverted index - get relevant documents
        self.icdr_lib.retrieve_relevant.argtypes = [
            ctypes.c_char_p,            # The query to be processed
            ctypes.c_int,               # The query processing algorithm (1: D-A-A-T/BMW)
            ctypes.c_int,               # The number of results to be retrieved
            ctypes.c_float,             # Minimum 'similarity' threshold
            ctypes.c_float,             # Maximum 'similarity' threshold
            IndexDataConnector          # A class with implicit pointers to the Index, the Entities and the Records.
        ]
        self.icdr_lib.retrieve_relevant.restype = ResultsTypeConnector

        # Submit a Record ID to the inverted index - get negative samples
        self.icdr_lib.retrieve_negative.argtypes = [
            ctypes.c_int,               # The ID of the Record for which we are searching for negatives.
            ctypes.c_int,               # The query processing algorithm (1: D-A-A-T/BMW)
            ctypes.c_int,               # The number of results to be retrieved
            ctypes.c_float,             # Minimum 'similarity' threshold
            ctypes.c_float,             # Maximum 'similarity' threshold
            IndexDataConnector          # A class with implicit pointers to the Index, the Entities and the Records.
        ]
        self.icdr_lib.retrieve_negative.restype = ResultsTypeConnector

        # Given a record collection get n negative samples for each record
        self.icdr_lib.retrieve_all_negatives.argtypes = [
            ctypes.c_int,               # The query processing algorithm (1: D-A-A-T/BMW)
            ctypes.c_int,               # The number of negative samples to be retrieved per record
            ctypes.c_float,             # Minimum 'similarity' threshold
            ctypes.c_float,             # Maximum 'similarity' threshold
            IndexDataConnector          # A class with implicit pointers to the Index, the Entities and the Records.
        ]
        self.icdr_lib.retrieve_all_negatives.restype = PairResultsTypeConnector

        # Write the inverted index and the accompanying data (Entities and Records) that are passed via a restype_class
        # pointer. Also write the parameters file. All four files will be written to the same output directory.
        self.icdr_lib.write_index.argtypes = [
            IndexDataConnector,         # A class with implicit pointers to the Index, the Entities and the Records.
            ctypes.c_char_p             # Output directory where the files will be written.
        ]
        self.icdr_lib.write_index.restype = None

        # Read the inverted index, the accompanying data (Entities and Records), and the parameters file from disk and
        # store them into a restype_class structure. All four files must reside into the same input directory.
        self.icdr_lib.read_index.argtypes = [
            ctypes.c_char_p             # Input directory where the input files are stored.
        ]
        self.icdr_lib.read_index.restype = IndexDataConnector

        # Deallocate the Resources used during Index construction including
        # the Index itself, the Entities and the Records.
        self.icdr_lib.destroy.argtypes = [
            IndexDataConnector          # A class with implicit pointers to the Index, the Entities and the Records.
        ]
        self.icdr_lib.destroy.restype = None

        # Display the Inverted Index and the accompanying data
        self.icdr_lib.display_index.argtypes = [
            IndexDataConnector          # A class with implicit pointers to the Index, the Entities and the Records.
        ]
        self.icdr_lib.display_index.restype = None

        # Display the Entities
        self.icdr_lib.display_entities.argtypes = [
            IndexDataConnector          # A class with implicit pointers to the Index, the Entities and the Records.
        ]
        self.icdr_lib.display_entities.restype = None

        # Compute several statistics and memory usage of the index and its data structures
        self.icdr_lib.compute_stats.argtypes = [
            ctypes.c_int,
            IndexDataConnector          # A class with implicit pointers to the Index, the Entities and the Records.
        ]
        self.icdr_lib.compute_stats.restype = None

        # Return the collection of entities
        self.icdr_lib.get_entities.argtypes = [
            IndexDataConnector          # A class with implicit pointers to the Index, the Entities and the Records.
        ]
        self.icdr_lib.get_entities.restype = EntitiesTypeConnector

        # Return the collection of records
        self.icdr_lib.get_records.argtypes = [
            IndexDataConnector          # A class with implicit pointers to the Index, the Entities and the Records.
        ]
        self.icdr_lib.get_records.restype = RecordsTypeConnector

    # Given a document collection, construct an Inverted Index, a list of Records and a list of matching entities.
    def build(self, input_file="", input_df=None, lex_size=300007, min_term_length=1, max_term_length=30,
              block_size=128, handle_hyphens=True, handle_slashes=True):

        """
        Given a document collection, construct an Inverted Index, a list of Records and a list of matching entities.

        :param input_file: The path of the input file where the document collection resides. The file must be a CSV with two columns: `(Record Title, Matching Entity ID)`
        :param input_df: A DataFrame that stores the document collection to be indexed. The DataFrame must have two columns: `(Record Title, Matching Entity ID)`
        :param lex_size: The initial value of the Lexicon structure of the Inverted Index. Defaults to 300007.
        :param min_term_length: The minimum length of a token to be included in the Index. Defaults to 1.
        :param max_term_length: The maximum length of a token to be included in the Index. Defaults to 30.
        :param block_size: The compression block size determines the number of postings in an Inverted List block. Defaults to 128.
        :param handle_hyphens: (True/False), defaults to True.
            * If True, the token is split with the hyphen as a separator. Then, the original token and the sub-tokens of the split process are included into the Index, provided that they meet they meet the `min_term_length` and `max_term_length` criteria.
            * If False, the token is not split, and it is inserted "as-is" in the Inverted Index.
        :param handle_slashes: Similar to `handle_hyphens` but with slashes instead of hyphens.
        """
        status = self.check_get_input(input_file, input_df)
        if status != 0:
            return

        self.index_data = self.icdr_lib.build(
            bytes(self.input_file, 'ASCII'),
            lex_size,
            min_term_length,
            max_term_length,
            block_size,
            handle_hyphens,
            handle_slashes
        )

    # Submit a query to the Inverted index and retrieve the results
    def retrieve_relevant(self, q, num_results=10, min_sim=0, max_sim=1.0, algorithm='bmw') -> pd.DataFrame:
        """
        Submit a query to the Inverted index and retrieve the results.

        :param q: The query to be submitted
        :param num_results: Number of results to be retrieved
        :param min_sim: Minimum similarity that a result must have with `q`
        :param max_sim: Maximum similarity that a result must have with `q`
        :param algorithm: The retrieval algorithm to be applied
        :return: A Pandas DataFrame with the results.
        """
        algo_param = 1
        if algorithm == 'bmw':
            algo_param = 1
        else:
            print("The algorithm may take values from ('bmw')")

        retrieved_results = self.icdr_lib.retrieve_relevant(
            bytes(q, 'ASCII'),
            algo_param,
            num_results,
            min_sim,
            max_sim,
            self.index_data
        )

        num_results = retrieved_results.num_results

        # A Python view of the array of results: ResultTypeConnectorClass * num_results
        result_array_type = ResultTypeConnector * num_results

        # Cast pointer to array type
        items_array = ctypes.cast(retrieved_results.results, ctypes.POINTER(result_array_type))

        r = []
        for i in range(num_results):
            r.append([items_array.contents[i].id, items_array.contents[i].text, items_array.contents[i].score])

        return pd.DataFrame(r, columns=['idx', 'txt', 'score'])

    # Submit a record to the Inverted index and retrieve num_results negative records
    def retrieve_negative(self, rid, num_results=10, min_sim=0, max_sim=1.0, algorithm='bmw') -> pd.DataFrame:
        """
        Submit a query to the Inverted index and retrieve the results.

        :param rid: The ID (row) of the Record for which the negative samples will be retrieved.
        :param num_results: Number of results to be retrieved
        :param min_sim: Minimum similarity that a result must have with `q`
        :param max_sim: Maximum similarity that a result must have with `q`
        :param algorithm: The retrieval algorithm to be applied
        :return: A Pandas DataFrame with the results.
        """
        algo_param = 1
        if algorithm == 'bmw':
            algo_param = 1
        else:
            print("The algorithm may take values from ('bmw')")

        retrieved_negatives = self.icdr_lib.retrieve_negative(
            rid,
            algo_param,
            num_results,
            min_sim,
            max_sim,
            self.index_data
        )

        num_results = retrieved_negatives.num_results

        # A Python view of the array of results: ResultTypeConnectorClass * num_results
        result_array_type = ResultTypeConnector * num_results

        # Cast pointer to array type
        items_array = ctypes.cast(retrieved_negatives.results, ctypes.POINTER(result_array_type))

        r = []
        for i in range(num_results):
            r.append([items_array.contents[i].id, items_array.contents[i].text, items_array.contents[i].score])

        return pd.DataFrame(r, columns=['idx', 'txt', 'score'])


    # Get the collection of Records
    def get_records(self) -> pd.DataFrame:
        """
        Get the collection of Records
        """
        recs = self.icdr_lib.get_records(
            self.index_data
        )

        num_recs = recs.num_records

        # A Python view of the array of records: RecordTypeConnectorClass * num_recs
        record_array_type = RecordTypeConnector * num_recs

        # Cast pointer to array type
        items_array = ctypes.cast(recs.records, ctypes.POINTER(record_array_type))

        r = []
        for i in range(num_recs):
            r.append([
                items_array.contents[i].id,
                items_array.contents[i].text.decode("utf-8"),
                items_array.contents[i].matching_entity.contents.code.decode("utf-8")
            ])

        return pd.DataFrame(r, columns=['idx', 'txt', 'matching_entity'])

    def write(self, path):
        if self.index_data:
            if self.check_output_dir(path) == 0:
                self.icdr_lib.write_index(
                    self.index_data,
                    bytes(path, 'ASCII')
                )
        else:
            print("No data to write")
            exit(-1)

    def read(self, path):
        if self.index_data:
            self.destroy()

        if self.check_input_dir(path) == 0:
            self.input_dir = path
            self.index_data = self.icdr_lib.read_index(
                bytes(self.input_dir, 'ASCII')
            )
        else:
            print("No valid path to read from")
            exit(-1)

    def display_index(self):
        if self.index_data:
            self.icdr_lib.display_index(self.index_data)
        else:
            print("No data to display")

    def display_entities(self):
        if self.index_data:
            self.icdr_lib.display_entities(self.index_data)
        else:
            print("No data to display")

    def display_records(self):
        if self.index_data:
            self.icdr_lib.display_records(self.index_data)
        else:
            print("No data to display")

    def compute_stats(self, d):
        if self.index_data:
            self.icdr_lib.compute_stats(d, self.index_data)
        else:
            print("No data to display")

    def destroy(self):
        if self.index_data:
            self.icdr_lib.destroy(self.index_data)
        else:
            print("No data to deallocate")

