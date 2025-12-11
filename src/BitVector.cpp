/// ICDR: Indexed Contrastive Data Retriever

/// BitVector Implementation File: A vector used to compress integers
/// Leonidas Akritidis, November 10th, 2011
/// //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ICDS_BITVECTOR_CPP
#define ICDS_BITVECTOR_CPP

#include "BitVector.h"

/// ///////////////////////////////////////////////////////////////////////////////////////////////
/// // BitVector Management Functions /////////////////////////////////////////////////////////////
/// ///////////////////////////////////////////////////////////////////////////////////////////////

/// Default Constructor, unusable
template <class T> BitVector<T>::BitVector() {
	this->vector = NULL;
}

/// Construct a BitVector of initveclen length
template <class T> BitVector<T>::BitVector(int32_t initveclen) {
	this->vector = new unsigned char[initveclen];

	if (this->vector == NULL) {
		printf("Error allocating bit vector....");
	}

	this->size = initveclen;
	this->last = 0;
	this->bit = 0;
	this->pos = 0;
	this->len = 0;
	this->cur = 0;
}

/// Read a Bitvector from file.
template <class T> int32_t BitVector<T>::read_vec(FILE *fp) {
	T length;
	size_t nread = 0;

	nread = fread(&this->len, sizeof(T), 1, fp);
	this->size = 4 * ((this->len / 8 + 8) / 4);

	length = this->len/8 + 1;
	this->vector = new unsigned char [this->size];

	if(this->vector != NULL) {
		nread = fread(this->vector, sizeof(unsigned char), length, fp);
		this->reset_vec();
	} else {
		return 0;
	}
	return sizeof(T) + nread;
}

/// Read a Bitvector from file.
template <class T> int32_t BitVector<T>::read_vec_2(FILE *fp) {
	T len;
	size_t nread = 0;
	uint32_t read_siz = 0;

	nread = fread(&this->len, sizeof(T), 1, fp);
	read_siz = 4 * ((this->len / 8 + 8) / 4);

	len = this->len/8 + 1;

//printf("\nRead Length = %d, Read Size = %d, Previous Size = %d\n", len, read_siz, this->size); fflush(NULL);
	if (read_siz >= this->size) {
		this->size = read_siz;
		delete [] this->vector;
		this->vector = new unsigned char [this->size];
	}

	if(this->vector != NULL) {
		nread = fread(this->vector, sizeof(unsigned char), len, fp);
		this->reset_vec();
	} else {
		return 0;
	}
	// printf("\nAfter Reaading %d\n", len); fflush(NULL);
	return sizeof(T) + nread;
}

/// Destructor
template <class T> BitVector<T>::~BitVector() {
	delete [] this->vector;
}

/// Return the BitVector's length
template <class T> inline T BitVector<T>::get_length() {
	return this->len;
}

/// Return the BitVector's size
template <class T> inline uint32_t BitVector<T>::get_size() {
	return this->size;
}

template <class T> inline uint32_t BitVector<T>::get_disk_size() {
	return sizeof(T) + (this->len/8 + 1) * sizeof(unsigned char);
}

/// Binary Logarithm Computation
template <class T> int32_t BitVector<T>::getlog2(uint32_t n) {
	uint32_t tmp;

	int32_t logn = 8 * sizeof(uint32_t) - 1;

	if ((n & 0xFF) == n) {
		logn = 7;
		n = n << ((8 * sizeof(uint32_t)) - 8);
	}

	for(; logn > 0; logn--) {
		tmp = n << 1;
		tmp = tmp >> 1;		/// the clear MSB
		if(tmp == n) {		/// if the top bit is not a 1
			n = n << 1;
		} else {
			break;
		}
	}
	return logn;
}

/// Construct a BitVector of initveclen length
template <class T> void BitVector<T>::init_vec(int32_t initveclen) {

	this->vector = new unsigned char[initveclen];

	if (this->vector == NULL) {
		printf("Error allocating bit vector....");
	}

	this->size = initveclen;
	this->last = 0;
	this->bit = 0;
	this->pos = 0;
	this->len = 0;
	this->cur = 0;
}

/// Reset the BitVector (place the pointer to the beginning of the vector)
template <class T> void BitVector<T>::reset_vec() {
	this->last = 0;
	this->pos = 0;
	this->bit = 0;
	this->cur = this->vector[0];
}

/// Set the BitVector Pointer into an exact bit in the vector
template <class T> inline void BitVector<T>::set_vec_ptr (T b) {
	this->last = 0;
	this->pos = b / 8;
	this->bit = b - this->pos * 8;
	this->cur = this->vector[this->pos];
}

/// Clear the Bitvector (reset the initial values)
template <class T> void BitVector<T>::clear_vec(int32_t initveclen) {
	this->size = initveclen;
	this->last = 0;
	this->bit = 0;
	this->pos = 0;
	this->len = 0;
	this->cur = 0;
}

/// Clear the Bitvector (reset the initial values)
template <class T> void BitVector<T>::clear_vec_2() {
	memset(this->vector, 0, this->size);
	this->last = 0;
	this->bit = 0;
	this->pos = 0;
	this->len = 0;
	this->cur = 0;
}

/// Expand the BitVector (allocate the double space to accommodate the additional data)
template <class T> void BitVector<T>::expand_vec() {
	unsigned char *newvec;
	uint32_t i;

	if ((newvec = new unsigned char [2 * this->size]) == NULL) {
		printf("Error expanding vector. Tried to allocate: %d bytes\n", 2 * this->size);
		fflush(NULL);
	} else {
//		printf ("Expanding BitVector...\n");
		for (i = 0; i < this->size; i++) {
			newvec[i] = this->vector[i];
		}

		this->size *= 2;

		delete [] this->vector;
		this->vector = newvec;
	}
}

/// Deallocate (Destroy the BitVector and release the occupied resources)
template <class T> void BitVector<T>::free_vec() {
	if (this->vector) {
		delete [] this->vector;
	}
	this->vector = NULL;

	this->size = 0;
	this->last = 0;
	this->bit = 0;
	this->pos = 0;
	this->len = 0;
	cur = 0;
	delete this;
}

/// Pad the Bitvector and force it to occupy an integral number of bytes.
template <class T> void BitVector<T>::pad_vec() {
	if (this->pos >= this->size) {
		this->expand_vec();
	}
	this->vector[this->pos] = this->cur;
	this->len = 8 * this->pos + this->bit;
}

/// Write the contents of a vector to file. Assumes that padvec has been called.
template <class T> void BitVector<T>::dump_vec(FILE *fp) {
	fwrite(&this->len, sizeof(T), 1, fp);
	fwrite(this->vector, sizeof(unsigned char), this->len/8 + 1, fp);
}


/// ///////////////////////////////////////////////////////////////////////////////////////////////
/// // Bit Readers and Bit Writers ////////////////////////////////////////////////////////////////
/// ///////////////////////////////////////////////////////////////////////////////////////////////

/// Put 1 (one) bit into the bitVector
template <class T> int32_t BitVector<T>::putbit (int32_t b) {
	unsigned char temp;
	temp = b & 0x1;
	temp = temp << (8 - this->bit - 1);
	this->cur = this->cur | temp;
	this->bit++;

	if (this->bit == 8) {

		if (this->pos >= this->size) {
			this->expand_vec();
		}
		this->vector[this->pos] = this->cur;
		this->cur = 0;
		this->pos++;
		this->bit = 0;
	}
	return(1);
}

/// Put a bundle of bits into the bitVector
template <class T> uint32_t BitVector<T>::putnbits (uint32_t number, uint32_t bits) {

	uint32_t n, int_size = 8 * sizeof(int32_t);
	uint32_t b, shift;
	b = bits;
	n = number;

	n = n << (int_size - bits);
	n = n >> (int_size - bits);

	for (shift = 8 - this->bit; bits >= shift; bits -= shift, shift = 8 - this->bit) {
		if (this->pos >= this->size) {
			this->expand_vec();
		}
		this->cur = this->cur | (n >> (bits - shift));
		this->vector[this->pos] = this->cur;

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

/// Read and retrieve 1 (one) bit from the BitVector
template <class T> int32_t BitVector<T>::getbit () {
	int32_t b;

	if (8 * this->pos + this->bit >= this->len) {
		return(-1);
	}

	b = ( this->cur >> ( 7 - this->bit ) ) & 0x1;
	this->bit++;

	if (this->bit == 8) {
		this->pos++;
		this->bit = 0;
		this->cur = this->vector[this->pos];
	}

	return(b);
}

/// Read and retrieve a bundle (num) of bits from the BitVector
template <class T> uint32_t BitVector<T>::getnbits(uint32_t num) {
	uint32_t shift;
	int32_t mask;
	uint32_t val = 0;
	unsigned char temp;

	for (shift = 8 - this->bit; num >= shift; num -= shift, shift = 8 - this->bit) {
		if (8 * this->pos + this->bit >= this->len) {
			return(-1);
		}

		mask = bv_masks[shift];
		val = (val << shift) | (this->cur & mask);
		this->bit = 0;
		this->pos++;
		this->cur = this->vector[this->pos];
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

/// Retrieve the last document ID value stored in the Bitvector
template <class T> inline uint32_t BitVector<T>::get_last() {
	return this->last;
}

/// Store the last document ID value in the Bitvector
template <class T> void BitVector<T>::set_last(uint32_t d) {
	this->last = d;
}

/// ///////////////////////////////////////////////////////////////////////////////////////////////
/// // BitVector Encoders / Decoders //////////////////////////////////////////////////////////////
/// ///////////////////////////////////////////////////////////////////////////////////////////////

/// Encode an integer according to unary
template <class T> int32_t BitVector<T>::Unary_Encode(int32_t n) {
	int32_t ret;

	ret = this->putnbits(0, n);
	ret += this->putnbits(1, 1);

	return (ret);
}

/// Decode a Unary encoded integer from the BitVector
template <class T> int32_t BitVector<T>::Unary_Decode() {
	int32_t ret = 0;

	for (; this->getbit() != 1; ret++);

	return (ret);
}

/// Encode an integer according to Elias Gamma
template <class T> int32_t BitVector<T>::EliasGamma_Encode(int32_t n) {
	int32_t mag;
	int32_t ret;

	if (n < 1 || n > 33554430) {
		return -1;
	}

	mag = getlog2((uint32_t) n);

	ret = this->putnbits(0, mag);

	ret += this->putnbits(n, mag + 1);

	return(ret);
}

/// Decode a Gamma encoded integer from the BitVector
template <class T> int32_t BitVector<T>::EliasGamma_Decode() {
	int32_t b, mag, val;

	for (mag = 0; (b = this->getbit()) == 0; mag++);

	if (b < 0) {
		return (-1);
	}

	val = this->getnbits(mag);

	if (val < 0) {
		return (-1);
	}

	return ((1 << mag) | val);
}

/// Encode an integer according to Elias Delta
template <class T> int32_t BitVector<T>::EliasDelta_Encode(int32_t n) {
	int32_t	mag;
	int32_t	ret;
	if (n < 1 || n > 33554430) {
		return -1;
	}

	mag = getlog2((uint32_t) n);

	ret = this->EliasGamma_Encode(mag + 1);
	ret += this->putnbits(n, mag);

	return(ret);
}

/// Decode a Delta encoded integer from the BitVector
template <class T> int32_t BitVector<T>::EliasDelta_Decode() {

	int32_t mag, val;
	mag = this->EliasGamma_Decode() - 1;

	if (mag < 0) {
		return(-1);
	}

	val = this->getnbits(mag);

	if (val < 0) {
		return(-1);
	}

	return ((1 << mag) | val);
}

/// Encode an integer according to Run Length
template <class T> int32_t BitVector<T>::RunLength_Encode(int32_t n) {
	int32_t ret;

	if (n <= this->last) {
		return(0);
	}

	ret = this->EliasDelta_Encode(n - this->last);
	this->last = n;

	return (ret);
}

/// Decode a Run Length encoded integer from the BitVector
template <class T> int32_t BitVector<T>::RunLength_Decode() {

	int32_t ret;

	ret = this->EliasDelta_Decode();

	if (ret > 0) {
		this->last += ret;
		return (this->last);
	} else {
		return (-1);
	}
}

/// Encode an integer according to Variable Byte. Offsets are on byte boundary, so can get value
/// directly from vector, which is faster than via putnbits.
template <class T> int32_t BitVector<T>::VByte_Encode(int32_t n) {

	int32_t ret = 0;

	if (n < 0) {
		printf("Error: putvbyte tried to put value %i", n);
		exit(1);
	}

	while (n >= 128) {
		if(this->pos >= this->size) {
			this->expand_vec();
		}

		this->vector[this->pos++] = (n & 0x7f) | 0x80;
		n >>= 7;
		ret += 8;
	}
	if (this->pos >= this->size) {
		this->expand_vec();
	}

	this->vector[this->pos++] = (n & 0x7f) & 0x7f;
	ret += 8;

	return ret;
}

/// Encode a long integer according to Variable Byte. Offsets are on byte boundary, so can we get
/// the value directly from vector, which is faster than via putnbits.
template <class T> int32_t BitVector<T>::VByte_Encode_Long(int64_t n) {

	int32_t ret = 0;

	if (n < 0) {
		printf("Error: putvbyte tried to put value %llu", n);
		exit(1);
	}

	while (n >= 128) {
		if(this->pos >= this->size) {
			this->expand_vec();
		}

		this->vector[this->pos++] = (n & 0x7f) | 0x80;
		n >>= 7;
		ret += 8;
	}
	if (this->pos >= this->size) {
		this->expand_vec();
	}

	this->vector[this->pos++] = (n & 0x7f) & 0x7f;
	ret += 8;

	return ret;
}

/// Decode a VByte encoded integer from the BitVector. Offsets are on byte boundary so we can get
/// the value directly from vector, which is faster than via getnbits. This is the fastest.
template <class T> int32_t BitVector<T>::VByte_Decode() {

	uint32_t ret = 0x0, count = 0, get = 0;

	/// For each time we get a group of 7 bits, need to left-shift the latest group by
	/// an additional 7 places, since the groups were effectively stored in reverse order.
	while (((get = this->vector[this->pos++]) & 0x80) == 0x80) {
		ret |= ((get & 0x7f) << count);
		count += 7;
	}

	/// Now get the final 7 bits, which need to be left-shifted by a factor of 7.
	ret |= (get & 0x7f) << count;

	return ret;
}

/// Decode a VByte encoded long long integer from the BitVector. Offsets are on byte boundary so we
/// can get the value directly from vector, which is faster than via getnbits. This is the fastest.
template <class T> uint64_t BitVector<T>::VByte_DecodeLong() {

	uint64_t ret = 0x0, count = 0, get = 0;

	/// For each time we get a group of 7 bits, need to left-shift the latest group by
	/// an additional 7 places, since the groups were effectively stored in reverse order.
	while (((get = this->vector[this->pos++]) & 0x80) == 0x80) {
		ret |= ((get & 0x7f) << count);
		count += 7;
	}

	/// Now get the final 7 bits, which need to be left-shifted by a factor of 7.
	ret |= (get & 0x7f) << count;

	return ret;
}

/// /////// TZP HIT ENCODING //////////////////////////
uint32_t TZP_Encode2 (uint32_t z1, uint32_t z2, uint32_t p1, uint32_t p2) {
	uint32_t n = 0;
	n = p2 << 19;
	n |= p1 << 6;
	n |= z2 << 3;
	n |= z1;
	return n;
}

void TZP_Encode (uint32_t *n, uint32_t z, uint32_t p, int32_t TURN) {

	if (TURN == 1) {
		*n = p << 19;
		*n |= z << 16;
	}
	if (TURN == 2) {
		*n |= p << 3;
		*n |= z;
	}

}

void TZP_Decode (uint32_t n, uint32_t *z1, uint32_t *z2, uint32_t *p1, uint32_t *p2) {
	*z1 = n & 0x7;
	*z2 = (n >> 3) & 0x7;
	*p1 = (n >> 6) & 0x1FFF;
	*p2 = (n >> 19) & 0x1FFF;
}

/// //////// BLOCKED TZP HIT ENCODING //////////////////////////
uint32_t encodeHit (uint32_t zone, uint32_t pos) {
	int32_t n = 0;
	n = pos << 3;
	n |= zone;
	return n;
}

void decodeHit (uint32_t n, uint32_t *z, uint32_t *p) {
	*z = n & 0x00000007;
	*p = n >> 3;
}

#endif
