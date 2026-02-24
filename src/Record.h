/**
ICDR: Indexed Contrastive Data Retriever

Record header file: An object used to represent an input record. An inverted index is constructed
on the titles of these records. Given a query, ICDR will return appropriate non-matching records.

L. Akritidis, 2026
*/

#ifndef ICDR_RECORD_H
#define ICDR_RECORD_H

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
        void read(FILE *, class Entities *);

        uint32_t get_id() const;
        char * get_text() const;
        uint32_t get_word_len() const;
        uint32_t get_uword_len() const;
        Entity * get_matching_entity() const;

        void set_id(uint32_t);
        void set_word_len(uint32_t);
        void set_uword_len(uint32_t);

        uint32_t get_footprint() const;
};

#endif //ICDR_RECORD_H
