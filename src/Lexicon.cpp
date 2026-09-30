/**
ICDR: Indexed Contrastive Data Retriever

Lexicon implementation file: A hash table that stores the distinct words (i.e. Word objects) of
a document collection.

L. Akritidis, 2026
*/

#ifndef ICDR_LEXICON_CPP
#define ICDR_LEXICON_CPP

#include "Lexicon.h"

/// The Lexicon's constructor accepts an InputParams object
Lexicon::Lexicon(class InputParams * pr) {
	uint32_t TableSize = pr->get_lexicon_table_size();
	this->hash_table = new Word * [TableSize];
	for (uint32_t i = 0; i < TableSize; i++) {
		this->hash_table[i] = NULL;
	}

	this->word_buffer_alloc_length = 32 * 1048576; /// Allocate 32MB of initial space for the word buffer
	this->word_buffer_length = 0;
	this->word_buffer = (char *)malloc(this->word_buffer_alloc_length * sizeof(char));

	this->min_term_length = pr->get_min_term_length();
	this->ColissionCount = 0;
	this->num_words = 0;
	this->tslots = TableSize;
	this->compression_block_size = pr->get_compression_block_size();
}

/// Destructor
Lexicon::~Lexicon() {
	uint32_t i;

	for (i = 0; i < this->tslots; i++) {
		if (this->hash_table[i]) {
			delete this->hash_table[i];
		}
	}

	if (this->hash_table) {
		delete [] this->hash_table;
		this->hash_table = NULL;
	}

	if (this->word_buffer) {
		free(this->word_buffer);
	}
}

/// The DJB2 Hash Function (Dan Bernstein)
uint32_t Lexicon::djb2(char * key) {
	unsigned long hash = 5381;
	int c;

	while ((c = *key++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

/// The KazLib Hash Function (https://github.com/blindchimp/kazlib)
uint32_t Lexicon::KazLibHash(char *key) {
	static unsigned long randbox[] = {
		0x49848f1bU, 0xe6255dbaU, 0x36da5bdcU, 0x47bf94e9U,
		0x8cbcce22U, 0x559fc06aU, 0xd268f536U, 0xe10af79aU,
		0xc1af4d69U, 0x1d2917b5U, 0xec4c304dU, 0x9ee5016cU,
		0x69232f74U, 0xfead7bb3U, 0xe9089ab6U, 0xf012f6aeU,
	};

	char *str = key;
	uint32_t acc = 0;

	while (*str) {
		acc ^= randbox[(*str + acc) & 0xf];
		acc = (acc << 1) | (acc >> 31);
		acc &= 0xffffffffU;
		acc ^= randbox[((*str++ >> 4) + acc) & 0xf];
		acc = (acc << 2) | (acc >> 30);
		acc &= 0xffffffffU;
	}
	return acc;
}

/// The JZ Hash Function (https://github.com/blindchimp/kazlib)
uint32_t Lexicon::JZHash(char * key, uint32_t len) {
	/// hash = hash XOR ((left shift L bits) AND (right shift R bits) AND key value)
	uint32_t hash = 1315423911;
	uint32_t i = 0;

	for (i = 0; i < len; key++, i++) {
		hash ^= ((hash << 5) + (*key) + (hash >> 2));
	}
	return hash;
}

/// Insert the word t in the hash table and associate a posting for the document d.
/// Return 1 upon success, 0 otherwise
uint32_t Lexicon::insert(uint32_t d, char * t, char * ret_c) {
	uint32_t res = 0, starting_offset = this->word_buffer_length;
	char c;

	class Word *p = NULL;

	if (strlen(t) <= this->min_term_length) {
		return 0;
	}

	/// Find the hash value of the input term
	uint32_t HashValue = this->djb2(t) & (this->tslots - 1);
	//printf("inserting %s, hash val: %d, starting offset: %d\n", t, HashValue, starting_offset);

	/// Now search in the hash table to check whether this term exists or not
	while (this->hash_table[HashValue] != NULL) {
		p = this->hash_table[HashValue];
		this->ColissionCount++;

		/// The term was found in the lexicon
		p->get_word_string(this->word_buffer, ret_c);
		if (strcmp(t, ret_c) == 0) {
			// printf("doc %d, %s was found in the lexicon at offset %d\n", d, ret_c, p->get_offset());
			res = p->insert_posting(d);
			return res; /// Return and exit
		}
		HashValue++;
	}

	/// The key wasn't found in the Lexicon.
	/// 1: Append the word's string into the big word buffer.
	c = t[0];
	while (c) {
		this->word_buffer[this->word_buffer_length++] = c;
		c = *(++t);
		if (this->word_buffer_length >= this->word_buffer_alloc_length) {
			// printf("Expanding...");
			this->word_buffer_alloc_length *= 2;
			this->word_buffer = (char *)realloc(this->word_buffer, this->word_buffer_alloc_length * sizeof(char));
		}
	}
	this->word_buffer[this->word_buffer_length++] = 0;

	/// 2: Create a new Word object and insert the posting.
	class Word * wrd = new Word(starting_offset);
	wrd->insert_posting(d);

	/// 3: Insert the Word object into the hash table
	this->hash_table[HashValue] = wrd;

	/// 4: Compute the table's load factor; expand if load_factor becomes high
	this->num_words++;
	float load_factor = (float)this->num_words / (float)this->tslots;
	// printf("Load factor (%d), %5.2f\n", this->num_words, load_factor);
	if(load_factor > 0.3) {
		//printf("Expanding");
		this->expand_table();
	}

	return 1;
}

/// Double the hash table size. Re-hash the existing keys and insert them into the new table.
void Lexicon::expand_table() {
	uint32_t i = 0, new_table_size = 2 * this->tslots, HashValue = 0;
	char word_buf[MAX_TERM_LENGTH];
	class Word * w = NULL;
	class Word ** new_table = new Word * [new_table_size];

	for (i = 0; i < new_table_size; i++) {
		new_table[i] = NULL;
	}

	/// Traverse the old table and move the non-NULL elements to the new table (after rehashing)
	for (i = 0; i < this->tslots; i++) {
		w = this->hash_table[i];
		if (w) {
			w->get_word_string(this->word_buffer, word_buf);
			// printf("Rehashing %s\n", word_buf); fflush(NULL);
			HashValue = this->djb2(word_buf) & (new_table_size - 1);
			while (new_table[HashValue] != NULL) {
				HashValue++;
			}
			new_table[HashValue] = w;
		}
	}

	/// Delete the old table (NOT the keys)
	delete [] this->hash_table;
	this->hash_table = new_table;
	this->tslots *= 2;
}

/// Insert the word t in the hash table and associate a posting for the document d.
/// Return 1 upon success, 0 otherwise
uint32_t Lexicon::insert(uint32_t offset, char * t, score_t idf, class InvertedList * ivl) {
	// char temp_buffer[MAX_TERM_LENGTH];
	class Word * wrd = new Word();
	wrd->set_offset(offset);
	wrd->set_idf(idf);
	wrd->set_ivl(ivl);

	wrd->get_word_string(this->word_buffer, t);

	uint32_t HashValue = this->djb2(t) & (this->tslots - 1);

	/// Now search in the hash table to check whether this term exists or not
	while (this->hash_table[HashValue] != NULL) {
/*
		this->hash_table[HashValue]->get_word_string(this->word_buffer, temp_buffer);
		printf("here (%s, %s)\n", t, temp_buffer); getchar();
		if (strcmp(temp_buffer, t) == 0) {
			printf("term was found! : %s\n", t);
			return 1;
		}
*/
		HashValue++;
	}

	/// Insert the term into an empty slot in the hash table
	this->hash_table[HashValue] = wrd;

	/// Compute the table's load factor; expand if load_factor becomes high
	this->num_words++;
	float load_factor = (float)this->num_words / (float)this->tslots;
	//printf("Load factor (%d), %5.2f\n", this->num_words, load_factor);
	if(load_factor > 0.3) {
		//printf("Expanding");
		this->expand_table();
	}

	//this->footprint += sizeof(Word) + sizeof(InvertedList) + (ivl->get_dwrd() + ivl->get_swrd()) * sizeof(uint32_t);
	//if (ivl->get_num_postings() > this->compression_block_size) {
	//	this->footprint += ivl->get_num_blocks(this->compression_block_size) * ivl->get_list_block_size();
	//}
	return 1;
}

/// Compress the inverted lists that are associated to the Lexicon nodes.
void Lexicon::compress_index(class Records * recs) {
	float idf = 0.0;
	class Word * q;
	uint32_t num_records = recs->get_num_records();

	for (uint32_t i = 0; i < this->tslots; i++) {
		if (this->hash_table[i] != NULL) {
			q = this->hash_table[i];
			idf = log( (num_records - q->get_freq() + 0.5f) / (q->get_freq() + 0.5f) + 1.0f);
			// idf = log10((score_t)num_records / (score_t)q->get_freq());
			q->set_idf(idf);
			q->compress_list(this->compression_block_size, recs);
			// printf("Word: %s, freq: %d, idf: %5.3f\n -- ", q->get_str(), q->get_freq(), idf);
			// q->display(); getchar();
		}
	}
}

/// Write the inverted index to disk.
void Lexicon::write_index(FILE * fp) {
	uint32_t num_words = 0;
	class Word * q;
	size_t nwrite = 0;

	if (fp) {
		nwrite = fwrite(&this->word_buffer_length, sizeof(uint32_t), 1, fp);
		if (nwrite == 0) { printf("Error writing the word buffer length\n"); fflush(NULL); }

		nwrite = fwrite(this->word_buffer, sizeof(char), this->word_buffer_length, fp);
		if (nwrite < this->word_buffer_length) { printf("Error writing the word buffer\n"); fflush(NULL); }

		for (uint32_t i = 0; i < this->tslots; i++) {
			if (this->hash_table[i] != NULL) {
				q = this->hash_table[i];
				num_words++;
				// printf("Writing %d: %s (%d)\n", num_words, q->get_str(), q->get_ivl()->get_num_postings());
				q->write(fp);
				q->write_list(fp, this->compression_block_size);
			}
		}
	} else {
		printf("Error opening index file for writing, aborting..."); fflush(NULL);
		exit(-1);
	}
	//printf("num_words = %d\n", num_words);
}

/// Read the inverted index from disk.
void Lexicon::read_index(FILE * fp, uint32_t block_size) {
	uint32_t ofs = 0;
	score_t idf = 0.0;
	size_t nread = 0;
	char temp_buffer[MAX_TERM_LENGTH];

	if (fp) {

		nread = fread(&this->word_buffer_length, sizeof(uint32_t), 1, fp);
		this->word_buffer_alloc_length = this->word_buffer_length + 1;
		this->word_buffer = (char *)realloc(this->word_buffer, (this->word_buffer_length + 1) * sizeof(char));
		nread = fread(this->word_buffer, sizeof(char), this->word_buffer_length, fp);
		this->word_buffer[this->word_buffer_length] = 0;

		while (!feof(fp)) {
			nread = fread(&ofs, sizeof(uint32_t), 1, fp);
			if (nread == 0) {
				break;
			}
			nread = fread(&idf, sizeof(score_t), 1, fp);
			//printf("%d (%d, %5.3f)\n", x++, ofs, idf); fflush(NULL);

			class InvertedList * ivl = new InvertedList();
			ivl->read(fp);

			this->insert(ofs, temp_buffer, idf, ivl);
		}
	} else {
		delete this;
		printf("Error opening index file for reading, aborting..."); fflush(NULL);
		exit(-1);
	}
}

/// Flush the lexicon's content to memory
void Lexicon::display() {
	class Word * q;

	for (uint32_t i = 0; i < this->tslots; i++) {
		if (this->hash_table[i] != NULL) {
			q = this->hash_table[i];
			q->display(this->word_buffer);
			printf("\n");
		}
	}
}

/// Compute the index statistics
void Lexicon::compute_stats() {
	uint32_t footprint = sizeof(Lexicon) + this->tslots * sizeof(Word *);
	class Word * q;

	for (uint32_t i = 0; i < this->tslots; i++) {
		if (this->hash_table[i] != NULL) {
			q = this->hash_table[i];
			footprint += q->get_footprint(this->compression_block_size);
		}
	}

	printf(" === Inverted file statistics ===================== \n");
	printf("\tMemory footprint: %5.2f MB\n", footprint / 1048576.0f);
	printf(" ================================================== \n\n");

	this->display_hash_table_performance();
}

void Lexicon::display_hash_table_performance() {
	printf(" === Lexicon hash table statistics ================ \n");
	printf("\tTable size (slots): %d\n", this->tslots);
	printf("\tNum keys: %d\n", this->num_words);
	printf("\tNum collisions: %d\n", this->ColissionCount);
	printf(" ================================================== \n\n");
}

/// Query Processing: Search the lexicon for a given term.
class Word * Lexicon::search(char * t, char * ret_c) {
	class Word *p;

	/// Find the hash value of the input term
	uint32_t HashValue = this->djb2(t) & (this->tslots - 1);
	//printf("inserting %s, hash val: %d, starting offset: %d\n", t, HashValue, starting_offset);

	/// Now search in the hash table to check whether this term exists or not
	while (this->hash_table[HashValue] != NULL) {
		p = this->hash_table[HashValue];

		/// The term was found in the lexicon
		p->get_word_string(this->word_buffer, ret_c);
		if (strcmp(t, ret_c) == 0) {
			return p; /// Return and exit
		}
		HashValue++;
	}

	// printf("The term '%s' was NOT found in the Lexicon!\n", t);
	return NULL;
}

inline char * Lexicon::get_word_buffer() { return this->word_buffer; }
#endif
