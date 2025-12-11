/// ICDR: Indexed Contrastive Data Retriever

/// BitVector Header File: A vector used to compress integers
/// Leonidas Akritidis, November 10th, 2011
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_BITVECTOR_H
#define ICDS_BITVECTOR_H

template <class T> class BitVector {
	private:
		unsigned char *vector;
		uint32_t size;
		T pos;
		uint32_t bit;
		unsigned char cur;
		T len;
		uint32_t last;

	private:
		void expand_vec();

	public:
		BitVector();
		BitVector(int32_t);
		~BitVector();
		T get_length();
		uint32_t get_size();
		uint32_t get_disk_size();

		/// BitVector Management
		int32_t getlog2(uint32_t);
		void init_vec(int32_t);
		void reset_vec();
		void set_vec_ptr(T);
		void clear_vec(int32_t);
		void clear_vec_2();
		void free_vec();
		void pad_vec();
		void dump_vec(FILE*);
		int32_t read_vec(FILE*);
		int32_t read_vec_2(FILE*);

		/// BitVector Data Reading and Writing
		int32_t putbit(int32_t);
		int32_t getbit();
		uint32_t putnbits(uint32_t, uint32_t);
		uint32_t getnbits(uint32_t);
		uint32_t get_last();
		void set_last(uint32_t);

		/// Encoding and Decoding the BitVector Data
		int32_t Unary_Encode(int32_t);
		int32_t Unary_Decode();
		int32_t EliasGamma_Encode(int32_t);
		int32_t EliasGamma_Decode();
		int32_t EliasDelta_Encode(int32_t);
		int32_t EliasDelta_Decode();
		int32_t RunLength_Encode(int32_t);
		int32_t RunLength_Decode();
		int32_t VByte_Encode(int32_t);
		int32_t VByte_Decode();

		int32_t VByte_Encode_Long(int64_t);
		int64_t VByte_Decode_Long();
		uint64_t VByte_DecodeLong();
};

#endif
