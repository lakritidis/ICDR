/// ICDS: Index-Assisted Contrastive Data Synthesizer

/// Query Implementation File: An object used to represent a query.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_QUERY_CPP
#define ICDS_QUERY_CPP

#include "Query.h"

/// Default constructor
Query::Query() :
	query_string(NULL),
	num_terms(0),
	query_terms(NULL),
	num_results(0),
	qparams(NULL),
	doc_info(NULL) {
}

/// Constructor
Query::Query(char * q, class InputParams * params, class Lexicon * lex, class Records * recs) :
	query_string(NULL),
	num_terms(0),
	query_terms(NULL),
	num_results(0),
	qparams(params),
	doc_info(recs) {

		int32_t i = 0, old_pos = -1, pos = 0, new_pos = 0, len = strlen(q);
		uint32_t y = 0;
		char term[params->get_max_term_length() + 1];

		this->query_string = new char[len + 1];
		strcpy(this->query_string, q);
		this->query_string[len] = 0;

		for (pos = 0; pos < len; pos++) {
			/// A "+" character has been found, or end of query string reached.
			if (q[pos] == 32 || pos == len - 1) {
				while (pos < len) {
					pos++;
					if (q[pos] != 32) {
						break;
					}
				}

				if (pos > 0 && pos < len) {
					new_pos = pos - 1;
				} else if (pos == 0) {
					new_pos = 0;
				} else if (pos == len) {
					new_pos = len;
				}

				y = 0;
				for (i = old_pos + 1; i < new_pos; i++) {
					if (y < params->get_max_term_length() - 1) {
						if (q[i] > 32 && q[i] < 127) {
							term[y] = q[i];
							y++;
						}
					} else {
						break;
					}
				}
				term[y] = 0;

				insert_term(term, y, lex);

				old_pos = new_pos;
			}
		}

		/// Sort the words array in ascending frequency order. The first element of the array is
		/// now the less frequent term, whereas its last element contains the most frequent term.
		qsort(this->query_terms, (size_t)(this->num_terms), sizeof(QueryWord**), Query::compare);
}

/// Destructor
Query::~Query() {
	if (this->query_string) {
		delete [] this->query_string;
		this->query_string = NULL;
	}

	if (this->query_terms) {
		for (uint32_t i = 0; i < this->num_terms; i++) {
			if (this->query_terms[i]) {
				delete this->query_terms[i];
			}
		}
		free(this->query_terms);
		this->query_terms = NULL;
	}
}

/// Insert a QueryWord into the local list of Query - compute the term's IDF
uint32_t Query::insert_term(char * tm, uint32_t l, class Lexicon * lex) {
	if (l <= this->qparams->get_min_term_length()) {
		return 0;
	}

	uint32_t i = 0;
	bool term_exists = false;
	class Word *temp;
	score_t idf = 0.0;

	/// Check if the term is already in the qterms array.
	for (i = 0; i < this->num_terms; i++) {
		if (strcmp(this->query_terms[i]->get_str(), tm) == 0) {
			term_exists = true;
			break;
		}
	}

	/// The term doesn't exist in the qterms array, so add it.
	if (!term_exists) {

		/// Search into the lexicon to obtain information related to the term (term frequency
		/// and address of the inverted list)
		temp = lex->search(tm);

		if (temp) {
			if (this->num_terms == 0) {
				this->query_terms = (class QueryWord **)malloc(sizeof(class QueryWord *));
			} else {
				this->query_terms = (class QueryWord **)realloc(this->query_terms,
					(this->num_terms + 1) * sizeof(class QueryWord *));
			}

			idf = log10 ( (score_t) this->doc_info->get_num_records() / (score_t) temp->get_ivl()->get_num_postings() );

			this->query_terms[this->num_terms] = new QueryWord(temp, this->qparams->get_compression_block_size());
			this->query_terms[this->num_terms]->set_idf(idf);
			this->num_terms++;
		}
	}
	return 1;
}


/// Checks if any of the query terms involved inverted lists is exhausted (i.e. we have checked all
/// the postings of the list). This flag signals the query processing termination (in case of
/// exhaustive evaulation)
bool Query::lists_exhausted() {
	for (uint32_t i = 0; i < this->num_terms; i++) {
		if (this->query_terms[i]->get_ivl_it()->is_exhausted()) {
			return true;
		}
	}
	return false;
}


/// This function returns which docID among those we are currently examining is the maximal
uint32_t Query::max_docID() {
	class InvertedListIterator * itr;
	uint32_t max_id = this->query_terms[0]->get_ivl_it()->get_cur_docID();

	for (uint32_t i = 0; i < this->num_terms; i++) {
		itr = this->query_terms[i]->get_ivl_it();
		if (itr->get_cur_docID() > max_id) {
			max_id = itr->get_cur_docID();
		}
	}

	return max_id;
}


/// This function returns which docID among those we are currently examining is the minimal
uint32_t Query::min_docID () {
	class InvertedListIterator * itr;
	uint32_t min_id = this->query_terms[0]->get_ivl_it()->get_cur_docID();

	for (uint32_t i = 0; i < this->num_terms; i++) {
		itr = this->query_terms[i]->get_ivl_it();
		if (itr->get_cur_docID() < min_id) {
			min_id = itr->get_cur_docID();
		}
	}

	return min_id;
}

/// Evaluate a document according to BM25 by summing the contribution of each term
void Query::score_BM25(uint32_t docID, class MaxHeap *heap) {
	uint32_t i = 0, doc_len = 0;
	score_t score = 0.0, K = 0.0, avg_doc_len = this->doc_info->get_avg_doc_len();

	/// BM25 Hyper-parameters
	score_t BM25_k1_PARAM = 1.2, BM25_k2_PARAM = 2.0, BM25_b_PARAM = 0.9;

	/// Retrieve the document length, the average document length and the corpus size.
	doc_len = this->doc_info->get_record(docID)->get_word_len();

	K = BM25_k2_PARAM * ( 1 - BM25_b_PARAM + (( BM25_b_PARAM * doc_len ) / avg_doc_len ));

	/// Create a new Result and compute its BM25 score (DocIDs are 1-indexed, the entries in
	/// this->doc_info are zero-indexed - that's why we subtract 1 from docID)
	class Record * corresp_record = this->doc_info->get_record(docID - 1);
	class Result *res = new Result(corresp_record->get_id(), corresp_record->get_text());

	/// Sum the contribution of each term
	for (i = 0; i < this->num_terms; i++) {
		score += this->query_terms[i]->get_ivl_it()->
			eval_posting_BM25(K, this->query_terms[i]->get_idf(), BM25_k1_PARAM);
	}

	res->set_score(score);

	/// Insert the Result into the MaxHeap
	heap->insert(res);

//	printf("\t Cand DocID: %d. Doc Length: %d, Avg Doc Length: %5.3f, K = %5.3f, BM25 Score: %5.3f\n",
//			docID, doc_len, avg_doc_len, K, score); fflush(NULL);
//	this->doc_info->get_record(docID - 1)->display();
}

/// Document-At-A-Time Query Processing
class Result * Query::process_DAAT() {

	uint32_t i = 0, dmax = 0;
//	printf("block size: %d", this->qparams->get_compression_block_size());
	bool proceed;
	class InvertedListIterator * list_iterator;
	class Result * r = NULL;
	class Result * res = NULL;

	if (this->num_terms > 0) {
		res = new Result [this->qparams->get_num_req_results()];

		/// Initialize scoring heap. This structure stores the best documents near its head.
		class MaxHeap *pq = new MaxHeap(this->query_terms[0]->get_ivl_it()->get_freq(), 200);

		/// ///////////////////////////////////////////////////////////////////////////////////////
		/// Phase 1: Decompress the first blocks of the inverted lists of all query terms. In case
		/// the term is short, (i.e. only one block exists) the buffers contain the entire list.
		/// ///////////////////////////////////////////////////////////////////////////////////////
		for (i = 0; i < this->num_terms; i++) {
			//printf("\nDecompressing block 0 for Term %d: %s, Frequency: %d. List Blocks: %d\n", i + 1,
			//	this->query_terms[i]->get_str(), this->query_terms[i]->get_freq(),
			//	this->query_terms[i]->get_ivl()->get_num_blocks(block_size));
			list_iterator = this->query_terms[i]->get_ivl_it();
			list_iterator->decode_docIDs(0);
			list_iterator->decode_frequencies(0);
		}

		/// ///////////////////////////////////////////////////////////////////////////////////////
		/// Phase 2: Begin DAAT list intersection; quickly evaluate the results by using
		/// "easy-to-find" data (document IDs, frequencies, document lengths etc.).
		/// ///////////////////////////////////////////////////////////////////////////////////////
		while (!this->lists_exhausted()) {

			dmax = this->max_docID();

			//printf("List 1 cur docID: %d. List 2 cur docID: %d. List 3 cur docID: %d. max docID: %d\n",
			//	this->query_terms[0]->get_ivl_it()->get_cur_docID(), this->query_terms[1]->get_ivl_it()->get_cur_docID(),
			//	this->query_terms[2]->get_ivl_it()->get_cur_docID(), dmax);

			for (i = 0; i < this->num_terms; i++) {
				list_iterator = this->query_terms[i]->get_ivl_it();
				// printf("\tList: %d - Seeking forward for docID = %d ... ", i, dmax);
				list_iterator->forward_seek(dmax);
				if (list_iterator->get_cur_docID() > dmax) {
					dmax = list_iterator->get_cur_docID();
				}
			}

			if (this->min_docID() == dmax) {
				// printf("\n == DocID %d IS A CANDIDATE === \n", dmax);

				/// This is a candidate result. Compute document's score here.
				score_BM25(dmax, pq);

				for (i = 0; i < this->num_terms; i++) {
					proceed = this->query_terms[i]->get_ivl_it()->next();
					if (!proceed) {
						break;
					}
				}
			}
		}

		/// ///////////////////////////////////////////////////////////////////////////////////////
		/// Phase 3: After the list intersection is completed, obtain the best documents from
		/// the MaxHeap's head. Deallocate all used memory (i.e. MaxHeap, uneeded results, etc.)
		/// ///////////////////////////////////////////////////////////////////////////////////////
		this->num_results = 0;
		while (!pq->is_empty()) {

			r = pq->remove_head();

			/// Keep only the number of requested results and delete the rest of them.
			if (this->num_results < this->qparams->get_num_req_results()) {
				res[this->num_results++] = (*r);
			}
			delete r;
		}

		delete pq;

		for (i = 0; i < this->num_terms; i++) {
			this->query_terms[i]->get_ivl_it()->clear();
		}
	} else {
		this->num_results = 0;
	}

	return res;
}


/// simd
/**
struct Block {
    docIDs[BLOCK_SIZE]          // sorted
    tf[BLOCK_SIZE]              // term frequency
    block_max_score             // max possible BM25 score (term) within this block
}

struct PostingList {
    blocks[]                    // array of blocks
    current_block_index
    pointer_within_block
}

struct QueryTerm {
    term
    pl : PostingList
    ub_global                   // global max contribution
}

function BM25_RANGE_SIMD_BMW(query_terms, lower_a, upper_b):

    // -------------------------
    // Preprocessing
    // -------------------------
    for qt in query_terms:
        qt.ub_global ← compute_global_bm25_upper_bound(qt.term)
        qt.pl.current_block ← 0
        qt.pl.pointer_within_block ← 0

    results ← empty list


    // -------------------------
    // Main Loop
    // -------------------------
    while true:

        // ----------------------------------------------------
        // 1. Pick pivot block using block max scores (BMW)
        // ----------------------------------------------------
        sort(query_terms by qt.pl.current_block’s next docID)

        cumulativeUB ← 0
        pivot_term ← null

        for qt in query_terms:
            block ← qt.pl.blocks[qt.pl.current_block]
            cumulativeUB += block.block_max_score
            if cumulativeUB >= lower_a:
                pivot_term ← qt
                break

        if pivot_term == null:
            break     // Cannot reach threshold a anymore


        // Candidate doc is the earliest doc in pivot block
        pivot_block ← pivot_term.pl.blocks[pivot_term.pl.current_block]
        candidate_doc ← pivot_block.docIDs[pivot_term.pl.pointer_within_block]


        // ----------------------------------------------------
        // 2. Align other posting lists to candidate_doc block
        // ----------------------------------------------------
        for qt in query_terms:
            while qt.pl.current_block < len(qt.pl.blocks):
                block ← qt.pl.blocks[qt.pl.current_block]

                if block.docIDs[block.end - 1] >= candidate_doc:
                    break

                // Skip entire block using block_max_score pruning
                if cumulativeUB - block.block_max_score < lower_a:
                    qt.pl.current_block++
                else:
                    break

            // Skip within block
            qt.pl.pointer_within_block ← lower_bound(
                block.docIDs, candidate_doc
            )

            if qt.pl.pointer_within_block == BLOCK_SIZE:
                qt.pl.current_block++
                qt.pl.pointer_within_block = 0

                if qt.pl.current_block == len(qt.pl.blocks):
                    goto END_LOOP


        // ----------------------------------------------------
        // 3. SIMD BM25 scoring of the candidate doc
        // ----------------------------------------------------
        exactScore ← 0

        for qt in query_terms:

            block ← qt.pl.blocks[qt.pl.current_block]

            idx ← qt.pl.pointer_within_block
            if block.docIDs[idx] == candidate_doc:

                // SIMD compute BM25 on a single element:
                // (using scalar fallback is fine for 1 element but
                // realistic engines do batch scoring)
                tf ← block.tf[idx]
                dl ← block.doc_length[idx]   // pre-packed
                norm ← 1 - b + b * dl/avgdl

                bm25_contrib ← qt.IDF *
                    (tf*(k1+1)) /
                    (tf + k1*norm)

                exactScore += bm25_contrib


        // ----------------------------------------------------
        // 4. Threshold & Range Filtering
        // ----------------------------------------------------
        if exactScore >= lower_a:

            if exactScore <= upper_b:
                append results, (candidate_doc, exactScore)

            // advance all lists positioned at candidate_doc
            for qt in query_terms:
                block ← qt.pl.blocks[qt.pl.current_block]
                if block.docIDs[qt.pl.pointer_within_block] == candidate_doc:
                    qt.pl.pointer_within_block++

                    if qt.pl.pointer_within_block == BLOCK_SIZE:
                        qt.pl.current_block++
                        qt.pl.pointer_within_block = 0

        else:
            // score didn't reach lower bound, move pivot forward
            pivot_term.pl.pointer_within_block++
            if pivot_term.pl.pointer_within_block == BLOCK_SIZE:
                pivot_term.pl.current_block++
                pivot_term.pl.pointer_within_block = 0

        // loop until exhaustion
    END_LOOP:

    return results
*/


/// Comparison function for QuickSorting the qterms array.
int32_t Query::compare(const void *A, const void *B) {
	class QueryWord *iA = *(class QueryWord **)A;
	class QueryWord *iB = *(class QueryWord **)B;
	return iA->get_ivl_it()->get_freq() - iB->get_ivl_it()->get_freq();
}

uint32_t Query::get_num_results() { return this->num_results; }
#endif
