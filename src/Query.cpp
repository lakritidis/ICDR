/**
ICDR: Index-Assisted Contrastive Data Synthesizer

Query implementation file: An object used to represent an input query. Implements various
query processing algorithms.

L. Akritidis, 2026
*/

#ifndef ICDR_QUERY_CPP
#define ICDR_QUERY_CPP

#include "Query.h"

/// Default constructor
Query::Query() :
	query_string(NULL),
	num_terms(0),
	query_terms(),
	num_results(0),
	qparams(NULL),
	doc_info(NULL) {
}

/// Constructor
Query::Query(char * q, class InputParams * params, class Lexicon * lex, class Records * recs) :
	query_string(NULL),
	num_terms(0),
	query_terms(),
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
				if (y > params->get_min_term_length()) {
					this->insert_term(term, y, lex);
				}
				old_pos = new_pos;
			}
		}
}

/// Destructor
Query::~Query() {
	if (this->query_string) {
		delete [] this->query_string;
		this->query_string = NULL;
	}

	for (uint32_t i = 0; i < this->query_terms.size(); i++) {
		delete this->query_terms[i];
	}
}

/// Insert a QueryWord into the local list of Query - compute the term's IDF
uint32_t Query::insert_term(char * tm, uint32_t l, class Lexicon * lex) {
	if (l <= this->qparams->get_min_term_length()) {
		return 0;
	}

	uint32_t i = 0;
	class Word *temp;

	/// Check if the term is already in the qterms array.
	for (i = 0; i < this->num_terms; i++) {
		if (strcmp(this->query_terms[i]->get_str(), tm) == 0) {
			return 1;
		}
	}

	/// Search into the lexicon to obtain information related to the term (term frequency
	/// and address of the inverted list)
	temp = lex->search(tm);
	if (temp) {
		QueryWord * qTerm = new QueryWord(temp, this->qparams->get_compression_block_size());
		qTerm->set_idf(temp->get_idf());
		this->query_terms.push_back(qTerm);
		this->num_terms++;
	}

	return 1;
}

/// Checks if any of the query terms involved inverted lists is exhausted (i.e. we have checked all
/// the postings of the list). This flag signals the query processing termination (in case of an
/// exhaustive evaluation).
bool Query::lists_exhausted() {
	for (uint32_t i = 0; i < this->num_terms; i++) {
		if (this->query_terms[i]->get_ivl_it()->is_exhausted()) {
			return true;
		}
	}
	return false;
}

/// Returns the maximum docID among those we are currently examining (cur_docIDs).
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

/// Returns the minimum docID among those we are currently examining (cur_docIDs).
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
void Query::score_BM25(uint32_t docID, class MaxHeap<Result> * heap) {
	uint32_t i = 0, doc_len = 0;
	score_t score = 0.0f, K = 0.0f, avg_doc_len = this->doc_info->get_avg_doc_len();

	/// Retrieve the document length, the average document length and the corpus size.
	doc_len = this->doc_info->get_record(docID - 1)->get_word_len();

	K = BM25_k2_PARAM * ( 1 - BM25_b_PARAM + (( BM25_b_PARAM * doc_len ) / avg_doc_len ));

	/// Create a new Result and compute its BM25 score (DocIDs are 1-indexed, the entries in
	/// this->doc_info are zero-indexed - that's why we subtract 1 from docID)
	class Record * corresp_record = this->doc_info->get_record(docID - 1);
	class Result *res = new Result(corresp_record->get_id(), corresp_record->get_text());

	/// Sum the contribution of each term
	for (i = 0; i < this->num_terms; i++) {
		score += this->query_terms[i]->get_ivl_it()->eval_posting_BM25(K, this->query_terms[i]->get_idf());
	}

	res->set_score(score);

	/// Insert the Result into the MaxHeap
	heap->insert(res);

//	printf("\t Cand DocID: %d. Doc Length: %d, Avg Doc Length: %5.3f, K = %5.3f, BM25 Score: %5.3f\n",
//			docID, doc_len, avg_doc_len, K, score); fflush(NULL);
//	this->doc_info->get_record(docID - 1)->display();
}

/// Document-At-A-Time Query Processing
class Result * Query::process_DAAT(uint32_t neg_docID) {
	// printf("\n=== DAAT QUERY PROCESSING (%d requested results)===\n", this->qparams->get_num_req_results());
	uint32_t t = 0, dmax = 0;
	bool proceed;
	class InvertedListIterator * list_iterator;
	class Result * r = NULL;
	class Result * res = NULL;
	score_t max_score_lists = 0.0f, max_score_th = 0.0f;

	if (this->num_terms > 0) {
		/// Sort the words array in ascending frequency order. The first element of the array is
		/// now the less frequent term, whereas its last element contains the most frequent term.
		sort(this->query_terms.begin(), this->query_terms.end(),
			[](const QueryWord * a, const QueryWord * b) {
				return a->get_c_ivl_it()->get_freq() > b->get_c_ivl_it()->get_freq();
			});

		res = new Result [this->qparams->get_num_req_results()];

		/// Initialize scoring heap. This structure stores the best documents near its head.
		class MaxHeap<Result> *pq = new MaxHeap<Result>(this->query_terms[0]->get_ivl_it()->get_freq());

		/// ///////////////////////////////////////////////////////////////////////////////////////
		/// Phase 1: Decompress the first blocks of the inverted lists of all query terms. In case
		/// the term is short, (i.e. only one block exists) the buffers contain the entire list.
		/// ///////////////////////////////////////////////////////////////////////////////////////
		for (t = 0; t < this->num_terms; t++) {
			list_iterator = this->query_terms[t]->get_ivl_it();
			list_iterator->decode_docIDs(0);
			list_iterator->decode_scores(0);
			max_score_lists += list_iterator->get_listMax_score();
			max_score_th += this->query_terms[t]->get_idf() * (BM25_k1_PARAM + 1);

			//printf("\tDecompressed block 0 ot term %d: %s, Frequency: %d. List Blocks: %d\n", t+1,
			//	this->query_terms[t]->get_str(), this->query_terms[t]->get_ivl_it()->get_num_postings(),
			//	this->query_terms[t]->get_ivl_it()->get_num_blocks());
			//list_iterator->display_skip_table(this->qparams->get_compression_block_size());

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

			for (t = 0; t < this->num_terms; t++) {
				list_iterator = this->query_terms[t]->get_ivl_it();
				// printf("\tList: %d - Seeking forward for docID = %d ... ", i, dmax);
				list_iterator->forward_seek(dmax, false);
				if (list_iterator->get_cur_docID() > dmax) {
					dmax = list_iterator->get_cur_docID();
				}
			}

			if (this->min_docID() == dmax) {
				// printf("\n == DocID %d IS A CANDIDATE === \n", dmax);

				/// This is a candidate result. Compute document's score here.
				score_BM25(dmax, pq);

				for (t = 0; t < this->num_terms; t++) {
					proceed = this->query_terms[t]->get_ivl_it()->next();
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

		for (t = 0; t < this->num_terms; t++) {
			this->query_terms[t]->get_ivl_it()->clear();
		}
	} else {
		this->num_results = 0;
	}

	return res;
}

/// BlockMaxWAND (BMW) Query Processing
class Result * Query::process_BMW(uint32_t neg_docID) {
	// printf("\n=== BMW QUERY PROCESSING (%d requested results)===\n", this->qparams->get_num_req_results());
	int32_t pivot = -1, matched = 0;
	uint32_t t = 0, cand_docID = 0, d = 0, n_terms = 0, negative_entity = 0;

	class InvertedListIterator * list_iterator;
	class Result * r = NULL, *res = NULL;
	class QueryWord * pivotTerm = NULL;
	score_t score = 0.0f, ub_sum = 0.0f, min_threshold = 0.0f, max_threshold = 0.0f;
	score_t max_score_lists = 0.0f, max_score_th = 0.0f;

	if (this->num_terms > 0) {
		if (neg_docID > 0) {
			negative_entity = this->doc_info->get_record(neg_docID - 1)->get_matching_entity()->get_id();
		}
		res = new Result[this->qparams->get_num_req_results()];

		/// Initialize scoring heap. This structure stores the best documents near its head.
		class MinHeap<Result> *pq = new MinHeap<Result>(this->qparams->get_num_req_results());

		/// ///////////////////////////////////////////////////////////////////////////////////////
		/// Phase 1: Decompress the first blocks of the inverted lists of all query terms. In case
		/// the term is short, (i.e. only one block exists) the buffers contain the entire list.
		/// ///////////////////////////////////////////////////////////////////////////////////////
		for (t = 0; t < this->num_terms; t++) {
			list_iterator = this->query_terms[t]->get_ivl_it();
			list_iterator->decode_docIDs(0);
			list_iterator->decode_scores(0);
			max_score_lists += list_iterator->get_listMax_score();
			max_score_th += this->query_terms[t]->get_idf() * (BM25_k1_PARAM + 1);

			//printf("\tDecompressed block 0 ot term %d: %s, Frequency: %d. List Blocks: %d\n", t+1,
			//	this->query_terms[t]->get_str(), this->query_terms[t]->get_ivl_it()->get_num_postings(),
			//	this->query_terms[t]->get_ivl_it()->get_num_blocks());
			//list_iterator->display_skip_table(this->qparams->get_compression_block_size());
		}

		max_threshold = max_score_lists * this->qparams->get_max_sim();
		min_threshold = max_score_lists * this->qparams->get_min_sim();
		//printf("\tMax Possible Score: %5.3f, Max Theoretical Bound: %5.3f\n", max_score_lists, max_score_th);
		//printf("\tMin Threshold: %5.3f, Max Threshold: %5.3f\n", min_threshold, max_threshold);

		for (;;) {
			// printf("\nNext iteration\n");

			/// Remove the query terms for which the inverted list has been exhausted.
			n_terms = this->num_terms;
			for (t = 0; t < n_terms; t++) {
				list_iterator = this->query_terms[t]->get_ivl_it();
				if (list_iterator->is_exhausted()) {
					// printf("\tTerm %d (%s) list has been exhausted at docID = %d", t,
					//	this->query_terms[t]->get_str(), list_iterator->get_cur_docID());
					delete this->query_terms[t];
					this->query_terms[t] = this->query_terms.back();
					this->query_terms.pop_back();
					this->num_terms--;
					//this->display_query_terms();
				}
			}

			/// Sort the remaining query terms in increasing current docID order.
			sort(this->query_terms.begin(), this->query_terms.end(),
				[](const QueryWord * a, const QueryWord * b) {
					return a->get_c_ivl_it()->get_cur_docID() < b->get_c_ivl_it()->get_cur_docID();
				});
			// this->display_query_terms();

			/// Pivoting step by using the block max scores (BMW)
			/// Update the threshold here
			if (!pq->is_empty()) {
				min_threshold = pq->get_head()->get_score();
			}

			ub_sum = 0.0f;
			pivot = -1;
			for (t = 0; t < this->num_terms; t++) {
				list_iterator = this->query_terms[t]->get_ivl_it();
				if (list_iterator->get_cur_block() >= list_iterator->get_num_blocks()) {
					getchar();
					return res;
				}

				ub_sum += list_iterator->get_cur_blockMax_score();
				if (ub_sum >= min_threshold) {
					pivot = t;
					break;
				}
			}

			/// No pivot term has been found. No documents can beat the minimum, so terminate early.
			if (pivot == -1) {
				//printf("Pivot=-1, ub_sum=%5.3f, thres=%5.3f, early termination...", ub_sum, min_threshold);
				break;
			}
			pivotTerm = this->query_terms[pivot];
			cand_docID = pivotTerm->get_ivl_it()->get_cur_docID();
			// if (cand_docID >= pivotTerm->get_ivl_it()->get_final_docID()) { break; }

			//printf("Pivot %d (%s), DocID=%d, ub_sum=%5.3f, threshold=%5.3f\n",
			//		pivot, pivotTerm->get_str(), cand_docID, ub_sum, min_threshold);

			/// If the smallest cur_docID is already >= candidate, then check the candidate.
			if (this->query_terms[0]->get_ivl_it()->get_cur_docID() == cand_docID) {
				score = 0.0f;
				matched = 0;

				for (t = 0; t < this->num_terms; t++) {
					list_iterator = this->query_terms[t]->get_ivl_it();
					d = list_iterator->get_cur_docID();
					if (d < cand_docID) {
						list_iterator->forward_seek(cand_docID, false);
						d = list_iterator->get_cur_docID();
					}
					if (d == cand_docID) {
						score += list_iterator->get_cur_score();
						matched++;
						//printf("\t\tCand DocID: %d in list %d: BM25 Score=%5.3f\n", cand_docID, t, score);
					}
				}

				if (matched > 0 && score >= min_threshold && score <= max_threshold) {
					if (negative_entity != this->doc_info->get_record(cand_docID - 1)->get_matching_entity()->get_id()) {
						pq->insert_replace(score, cand_docID, this->doc_info);
					}
				}

				/// Advance all lists past candidate.
				for (t = 0; t < this->num_terms; t++) {
					list_iterator = this->query_terms[t]->get_ivl_it();
					if (list_iterator->get_cur_docID() == cand_docID) {
						list_iterator->forward_seek(cand_docID + 1, false);
					}
				}
			} else {
				/// Advance lists with docid < candidate to candidate.
				for (t = 0; t < this->num_terms; t++) {
					list_iterator = this->query_terms[t]->get_ivl_it();
					if (list_iterator->get_cur_docID() < cand_docID) {
						list_iterator->forward_seek(cand_docID, false);
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

		for (t = 0; t < this->num_terms; t++) {
			this->query_terms[t]->get_ivl_it()->clear();
		}
	} else {
		this->num_results = 0;
	}

	return res;
}

/// Display the query terms and the current state of the InvertedListIterator during query processing
void Query::display_query_terms() {
	class QueryWord * qt = NULL;
	class InvertedListIterator * it = NULL;

	for (uint32_t i = 0; i < this->num_terms; i++) {
		qt = this->query_terms[i];
		it = qt->get_ivl_it();

		printf("\tTerm %d: %s, Freq: %d, CurDocID: %d, CurBlock: %d, CurOffset: %d, BlockMax: %5.3f\n",
			i, qt->get_str(), it->get_num_postings(), it->get_cur_docID(), it->get_cur_block(), it->get_cur_offset(),
			it->get_cur_blockMax_score());
	}
}


/// Comparison function for QuickSorting the qterms array in increasing current docID order.
int32_t Query::compare_qterms(const void *A, const void *B) {
	class QueryWord *iA = *(class QueryWord **)A;
	class QueryWord *iB = *(class QueryWord **)B;
	return iA->get_ivl_it()->get_cur_docID() - iB->get_ivl_it()->get_cur_docID();
}

uint32_t Query::get_num_results() { return this->num_results; }
#endif
