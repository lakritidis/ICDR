/// ICDR: Indexed Contrastive Data Retriever

/// Lexicon Implementation File: A hast table that stores the distinct words (i.e. Word objects)
/// of a document collection.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_LEXICON_CPP
#define ICDS_LEXICON_CPP

#include "Lexicon.h"

/// The Lexicon's constructor accepts an InputParams object
Lexicon::Lexicon(class InputParams * pr) {
	uint32_t TableSize = pr->get_lexicon_table_size();
	this->hash_table = new Word * [TableSize];
	for (uint32_t i = 0; i < TableSize; i++) {
		this->hash_table[i] = NULL;
	}

	this->min_term_length = pr->get_min_term_length();
	this->ColissionCount = 0;
	this->num_words = 0;
	this->num_chains = 0;
	this->tslots = TableSize;
	this->mask = TableSize - 1;
	this->footprint = TableSize * sizeof(Word*);
	this->compression_block_size = pr->get_compression_block_size();
}

/// Destructor
Lexicon::~Lexicon() {
	uint32_t i;
	class Word * q;

	for (i = 0; i < this->tslots; i++) {
		while (this->hash_table[i] != NULL) {
			q = this->hash_table[i]->get_next();
			delete this->hash_table[i];
			this->hash_table[i] = q;
		}
	}

	if (this->hash_table) {
		delete [] this->hash_table;
		this->hash_table = NULL;
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
uint32_t Lexicon::JZHash(char* key, uint32_t len) {
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
uint32_t Lexicon::insert(uint32_t d, char * t) {
	uint32_t res = 0;

	if (strlen(t) <= this->min_term_length) {
		return 0;
	}

	/// Find the hash value of the input term
	uint32_t HashValue = this->djb2(t) & this->mask;

	/// Now search in the hash table to check whether this term exists or not
	if (this->hash_table[HashValue] != NULL) {
		class Word *q, *p;
		p = this->hash_table[HashValue];

		/// Traverse the linked list that represents the chain.
		for (q = this->hash_table[HashValue]; q != NULL; q = q->next) {
			if (strcmp(q->w, t) == 0) {

				res = q->insert_posting(d);

				/// Move-To-Front. Move the node to the front of the chain to accelerate the
				/// searches of frequent terms. If the node is already in the chain's head we don't
				/// need to apply it
				if (q != this->hash_table[HashValue]) {
					/// Connect the previous element to the next.
					p->next = q->next;

					/// The old head goes back one position.
					q->next = this->hash_table[HashValue];

					/// Change head.
					this->hash_table[HashValue] = q;
				}
				return res; /// Return and exit
			}
			/// p stores the next node of the chain in the next step of the loop, q becomes q->next
			p = q;
		}
	} else {
		this->num_chains++;
	}

	/// The key hasn't been found on a non-empty chain, or the chain is empty.
	this->num_words++;

	/// Create a new posting and re-assign the linked list's head
	class Word * wrd = new Word(t);
	wrd->insert_posting(d);

	/// Reassign the chain's head
	wrd->next = this->hash_table[HashValue];
	this->hash_table[HashValue] = wrd;

	return 1;
}

/// Insert the word t in the hash table and associate a posting for the document d.
/// Return 1 upon success, 0 otherwise
uint32_t Lexicon::insert(char * t, class InvertedList * ivl) {
	/// Find the hash value of the input term
	uint32_t HashValue = this->djb2(t) & this->mask;

	/// Now search in the hash table to check whether this term exists or not
	if (this->hash_table[HashValue] != NULL) {
		class Word *q;

		/// Traverse the linked list that represents the chain.
		for (q = this->hash_table[HashValue]; q != NULL; q = q->next) {
			if (strcmp(q->w, t) == 0) {
				printf("term was found! : %s\n", t);
				return 1; /// Return and exit
			}
		}
	} else {
		this->num_chains++;
	}

	/// The key hasn't been found on a non-empty chain, or the chain is empty.
	this->num_words++;

	/// Create a new posting and re-assign the linked list's head
	class Word * wrd = new Word();
	wrd->set_word_string(t);
	wrd->set_ivl(ivl);

	/// Reassign the chain's head
	wrd->next = this->hash_table[HashValue];
	this->hash_table[HashValue] = wrd;

	return 1;
}

/// Compress the inverted lists that are associated to the Lexicon nodes.
void Lexicon::compress_index() {
	class Word * q;

	for (uint32_t i = 0; i < this->tslots; i++) {
		if (this->hash_table[i] != NULL) {
			for (q = this->hash_table[i]; q != NULL; q = q->get_next()) {
				// printf("Word: %s -- ", q->get_word_string());
				q->compress_list(this->compression_block_size);
				// q->display(); getchar();
			}
		}
	}
}

/// Write the inverted index to disk.
void Lexicon::write_index(FILE * fp) {
	uint32_t num_words = 0;
	class Word * q;
	if (fp) {
		for (uint32_t i = 0; i < this->tslots; i++) {
			if (this->hash_table[i] != NULL) {
				for (q = this->hash_table[i]; q != NULL; q = q->get_next()) {
					num_words++;
					// printf("Writing %d: %s (%d)\n", num_words, q->get_str(), q->get_ivl()->get_num_postings());
					q->write(fp);
					q->write_list(fp, this->compression_block_size);
				}
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
	uint32_t tl = 0;
	ssize_t nread = 0;

	if (fp) {
		while (!feof(fp)) {
			nread = fread(&tl, sizeof(uint32_t), 1, fp);
			if (nread == 0) {
				break;
			}

			char *term = new char[tl + 1];

			nread = fread(term, sizeof(char), tl, fp);
			term[tl] = 0;

			class InvertedList * ivl = new InvertedList();
			ivl->read(fp);

			this->insert(term, ivl);
		}
	} else {
		delete this;
		printf("Error opening index file for reading, aborting..."); fflush(NULL);
		exit(-1);
	}
}

/// Decompress the (compressed) inverted lists that are associated to the Lexicon nodes.
void Lexicon::decompress_index() {
}

/// Flush the lexicon's content to memory
void Lexicon::display() {
	class Word * q;

	for (uint32_t i = 0; i < this->tslots; i++) {
		if (this->hash_table[i] != NULL) {
			for (q = this->hash_table[i]; q != NULL; q = q->get_next()) {
				q->display();
				printf("\n");
//				getchar();
			}
		}
	}
}

/// Query Processing: Search the lexicon for a given term.
class Word * Lexicon::search(char * t) {
	/// Find the hash value of the input term
	uint32_t HashValue = this->djb2(t) & this->mask;

	/// Now search in the hash table to check whether this term exists or not
	if (this->hash_table[HashValue] != NULL) {
		class Word *q;

		/// Traverse the linked list that represents the chain.
		for (q = this->hash_table[HashValue]; q != NULL; q = q->next) {
			if (strcmp(q->w, t) == 0) {
				return q; /// The word was found in the Lexicon. Return it.
			}
		}
	}
	printf("term was NOT found! : %s\n", t);
	return NULL;
}

#endif
