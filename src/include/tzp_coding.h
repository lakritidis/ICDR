/// ///////////////////////////////////////////////////////////////////////////////////////////////
/// HEADIR Large Scale Retrieval System
///
/// TZP coder descriptor (encode/decode)
///
/// Leonidas Akritidis, November 7th, 2011
/// ///////////////////////////////////////////////////////////////////////////////////////////////

#include "coding.h"

class tzp_coding {
	public:
		tzp_coding();
		int32_t Compression(uint32_t *, uint32_t *, int32_t, uint32_t*);
		int32_t Decompression(uint32_t *, uint32_t *, int32_t, uint32_t, int32_t);
		int get_type();
		void set_size(int size);
		void pad_vec(unsigned char*);

	private:
		int32_t type;
		int32_t size;
		int32_t bit;
		int64_t pos;
		int64_t len;
		unsigned char cur;
		unsigned int find_bits(unsigned int *, int);
		void set_ptr(unsigned char*, unsigned long long int);
		unsigned int getnbits(unsigned char *, int);
		unsigned int get8bits(unsigned char *);
		unsigned int putnbits(unsigned char *, unsigned int, int);
};

tzp_coding::tzp_coding() {
	this->type = 6;
	this->bit = 0;
	this->pos = 0;
	this->len = 0;
}

int tzp_coding::get_type() {
	return type;
}

void tzp_coding::set_size(int size) {
	return;
}

void tzp_coding::set_ptr(unsigned char *vector, unsigned long long int b) {
	this->pos = b / 8;
	this->bit = b - this->pos * 8;
	this->cur = vector[this->pos];
}

unsigned int tzp_coding::putnbits (unsigned char *vector, unsigned int number, int bits) {

	unsigned int n, int_size = 32;
	int b, shift;
	b = bits;
	n = number;

	n = n << (int_size - bits);
	n = n >> (int_size - bits);

	for (shift = 8 - this->bit; bits >= shift; bits -= shift, shift = 8 - this->bit) {
		this->cur = this->cur | (n >> (bits - shift));
		vector[this->pos] = this->cur;

		n = n << (int_size - (bits - shift));
		n = n >> (int_size - (bits - shift));

		this->bit = 0;
		this->pos++; /// move to next byte

		this->cur = 0;
	}

	if (bits > 0) {
		this->cur = this->cur | (n << (shift - bits));
		this->bit += bits;
	}
	return(b);
}

void tzp_coding::pad_vec(unsigned char *vector) {
	vector[this->pos] = this->cur;
	this->len = 8 * this->pos + this->bit;
}

/// Read and retrieve a bundle (num) of bits from the BitVector
unsigned int tzp_coding::getnbits(unsigned char *vector, int num) {
	int shift = 0;
	int mask;
	unsigned int val;
	unsigned char temp;

    val = 0;

	for (shift = 8 - this->bit; num >= shift; num -= shift, shift = 8 - this->bit) {
//		printf("this->bit=%d, num=%d, shift=%d\n", this->bit, num, shift);
		if (8 * this->pos + this->bit >= this->len) {
			return(-1);
		}

		mask = bv_masks[shift];
		val = (val << shift) | (this->cur & mask);
		this->bit = 0;
		this->pos++;
		this->cur = vector[this->pos];
//		printf("this->bit=%d, num=%d, shift=%d\n", this->bit, num, shift);
    }

	if (num > 0) {
		temp = this->cur;
		temp = temp << this->bit;
		temp = temp >> ( 8 - num );
		val = (val << num) | temp;
		this->bit += num;
	}

    return ((val < 0) ? -1 : val);
}

/// Read and retrieve a bundle (num) of bits from the BitVector
unsigned int tzp_coding::get8bits(unsigned char *vector) {
	unsigned int val;

	val = this->cur & 0xFF;
	this->bit = 0;
	this->pos++;
	this->cur = vector[this->pos];

    return val;
}

unsigned int tzp_coding::find_bits(unsigned int *in, int size) {
	unsigned int max = in[0], bits = 0;

	for (int _i = 0; _i < size; _i++) {
		if (in[_i] > max) {
			max = in[_i];
		}
	}

	bits = ceil(log2(max));
	return bits;
}


int32_t tzp_coding::Compression(uint32_t* input, uint32_t* output, int32_t size, uint32_t *nbits) {

	int32_t i = 0, ret = 0;
	*nbits = find_bits(input, size);
	unsigned char *tmp = (unsigned char*)output;

	this->bit = 0;
	this->pos = 0;
	this->cur = 0;

	for (i = 0; i < size; i++) {
//		printf("%d\n", input[i]);
		ret += putnbits(tmp, input[i], *nbits);
	}

	this->len += 32 * (ret / 32) + 32;
	tmp[this->pos] = this->cur;
//	printf("Length = %d\n", this->len);

//	printf("Padded Length = %d bits. CmprSize = %d ints, Num Of Bits = %d - Return = %d\n",
//		this->len, (ret / 32) + 1, (*nbits) * size, ret);
	return (ret / 32) + 1;
}

/*
int32_t tzp_coding::Compression(uint32_t* input, uint32_t* output, int32_t size, uint32_t *nbits) {

	int32_t i = 0, ret = 0;
	*nbits = find_bits(input, size);
	unsigned char *tmp = (unsigned char*)output;

	this->bit = 0;
	this->pos = 0;
	this->cur = 0;

	for (i = 0; i < size; i++) {
//		printf("%d\n", input[i]);
		ret += putnbits(tmp, input[i], *nbits);
	}

	if (ret % 32 == 0) {
		this->len += ret;
	} else {
		this->len += 32 * (ret / 32) + 32;
	}

	tmp[this->pos] = this->cur;
//	printf("Length = %d\n", this->len);

//	printf("Padded Length = %d bits. CmprSize = %d ints, Num Of Bits = %d - Return = %d\n",
//		this->len, (ret / 32) + 1, (*nbits) * size, ret);
	if (ret % 32 == 0) {
		return ret / 32;
	} else {
		return (ret / 32) + 1;
	}
}
*/
// Starting from bit b, decode size integers by reading nb bits for each
int32_t tzp_coding::Decompression(uint32_t* input, uint32_t* output, int32_t size,
	uint32_t start, int32_t num_bits) {

		unsigned char *tmp_in = (unsigned char *)input;
		int32_t _p = start / 8;
		int32_t _b = start - _p * 8, shift = 0, mask = 0, i = 0, s = size, num = num_bits;
		unsigned char _c = tmp_in[_p], temp;
		uint32_t val = 0;
		this->len = 10000000000ULL;

		for (i = 0; i < s; i++) {

			num = num_bits;
			val = 0;

			for (shift = 8 - _b; num >= shift; num -= shift, shift = 8 - _b) {
				if (8 * _p + _b >= this->len) {
					return (-1);
				}

				mask = bv_masks[shift];
				val = (val << shift) | (_c & mask);
				_b = 0;
				_p++;
				_c = tmp_in[_p];
			}

			if (num > 0) {
				temp = _c;
				temp = temp << _b;
				temp = temp >> ( 8 - num );
				val = (val << num) | temp;
				_b += num;
			}

			output[i] = val;
		}

//		printf("Decompression: Num Bits = %d, Size = %d. ", num_bits, size);
		return (num_bits * size) / 32 + 1;
}



/*
// Starting from bit b, decode size integers by reading nb bits for each
int tzp_coding::Decompression(unsigned int* input, unsigned int* output, int size,
	unsigned long long int start, int num_bits) {

		unsigned char *tmp_in = (unsigned char *)input;
		long long int _p = start / 8;
		int _b = start - _p * 8, shift = 0, mask = 0, i = 0, s = size;
		unsigned char _c = tmp_in[_p], temp;
		unsigned int val = 0, num = num_bits;

		for (i = 0; i < s; i++) {

			num = num_bits;
			val = 0;

			for (shift = 8 - _b; num >= shift; num -= shift, shift = 8 - _b) {
				if (8 * _p + _b >= this->len) {
					return(-1);
				}

				mask = bv_masks[shift];
				val = (val << shift) | (_c & mask);
				_b = 0;
				_p++;
				_c = tmp_in[_p];
			}

			if (num > 0) {
				temp = _c;
				temp = temp << _b;
				temp = temp >> ( 8 - num );
				val = (val << num) | temp;
				_b += num;
			}

			output[i] = val;
		}
		return num_bits * size;
}
*/
/*
// Starting from bit b, decode size integers by reading nb bits for each
int tzp_coding::Decompression(unsigned int* input, unsigned int* output, int size,
	unsigned long long int start, int num_bits) {

		unsigned char *tmp_in = (unsigned char *)input;
		this->pos = start / 8;
		this->bit = start - this->pos * 8;
		int shift = 0, mask = 0, i = 0, s = size;
		this->cur = tmp_in[this->pos];

		for (i = 0; i < s; i++) {
			getnbits(tmp_in, s * num_bits);
		}
		return num_bits * size;
}
*/
