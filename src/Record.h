/// ICDR: Indexed Contrastive Data Retriever

/// Record Header File: An object used to represent an input record. An inverted index
/// is constructed on the titles of these records. Given a query, ICDS will return appropriate
/// matching or non-matching records.
/// Leonidas Akritidis, October 16th, 2025
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_RECORD_H
#define ICDS_RECORD_H

class Record {
    uint32_t id;
    char * text;
    uint32_t word_len;   /// Number of words
    uint32_t uword_len;  /// Number of unique words
    Entity * matching_entity{};

    public:
        Record();
        Record(uint32_t, const char *, Entity *);
        ~Record();

        void display() const;
        void write(FILE *);
        void read(FILE *);

        uint32_t get_id() const;
        char * get_text() const;
        uint32_t get_word_len() const;
        uint32_t get_uword_len() const;
        Entity *get_matching_entity() const;

        void set_id(uint32_t);
        void set_word_len(uint32_t);
        void set_uword_len(uint32_t);
};

#endif //IACTS_RECORD_H
