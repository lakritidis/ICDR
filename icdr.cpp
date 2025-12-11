#include "stdio.h"
#include "stdlib.h"
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <assert.h>
#include <vector>

#include "src/include/coding_factory.cpp"
typedef float score_t;

coding_factory cf;
coding * PFOR_CODER = cf.get_coder(1);
coding * VBYTE_CODER = cf.get_coder(2);


#include "src/InputParams.cpp"

#include "src/BitVector.cpp"
#include "src/InvertedList.cpp"
#include "src/InvertedListIterator.cpp"
#include "src/Word.cpp"
#include "src/QueryWord.cpp"

#include "src/Entities.cpp"
#include "src/Records.cpp"
#include "src/Lexicon.cpp"
#include "src/Result.cpp"
#include "src/MaxHeap.cpp"
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
