#include "stdio.h"
#include "stdlib.h"
#include <bits/stdc++.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <chrono>
#include <time.h>
#include <dirent.h>
#include <assert.h>
#include <vector>
#include <type_traits>

#include "src/include/coding_factory.cpp"
typedef float score_t;
const uint32_t MAX_TERM_LENGTH = 100;

/// Integer encoders/decoders
coding_factory cf;
coding * PFOR_CODER = cf.get_coder(1);
coding * VBYTE_CODER = cf.get_coder(2);

/// BM25 Hyper-parameters
score_t BM25_k1_PARAM = 1.5, BM25_k2_PARAM = 1.5, BM25_b_PARAM = 0.75;
float SHL_SCALER = 128.0f;
float LOL_SCALER = 512.0f;

#include "src/InputParams.cpp"

#include "src/Entities.cpp"
#include "src/Records.cpp"

#include "src/InvertedList.cpp"
#include "src/InvertedListIterator.cpp"
#include "src/Word.cpp"
#include "src/QueryWord.cpp"
#include "src/InvertedListBuffer.cpp"

#include "src/Lexicon.cpp"
#include "src/PairResult.cpp"
#include "src/Heap.cpp"
#include "src/Query.cpp"
#include "src/InputData.cpp"


struct rettype {
	class Lexicon * lex;
	class Entities * ents;
	class Records * recs;
	class InputParams * in_params;
};

struct resulttype {
	unsigned int num_results;
	class Result * results;
};

struct pairresulttype {
	unsigned int num_results;
	class PairResult * results;
};

struct negativesamplestype {
	unsigned int num_results;
	class Result * results;
};

struct entitytype {
	unsigned int num_entities;
	class Entity * entities;
};

struct recordtype {
	unsigned int num_records;
	class Record * records;
};
