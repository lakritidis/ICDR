#undef DEBUG
#undef CHECK

typedef struct compressedBuffer {
    unsigned char *buffer;  // the compressed byte sequence
    int	size;				// the buffer's size
    int	pos;				// pointer to a byte within the buffer
};

// Initialize an empty compresssed byte sequence.
void initbuf(compressedBuffer *cb, int size) {
	cb->buffer = (unsigned char*)malloc(size * sizeof(unsigned char));

	if (cb->buffer == NULL) { printf("Error allocating buffer...."); }
	cb->size = size;
	cb->pos = 0;
}

// Reset the pos pointer to the begining of the buffer.
void resetbuf(compressedBuffer *cb) {
	cb->pos = 0;
}

// Expand the buffer by doubling its size.
void doublebuf(compressedBuffer *cb) {
	unsigned char *newbuf;
	int i;

	if ((newbuf = (unsigned char*)malloc(2 * cb->size * sizeof(unsigned char))) == NULL) {
		printf("Error expanding Buffer. Tried to allocate: %ld bytes\n",
			2 * cb->size * sizeof(unsigned char));
	} else {
		for (i = 0; i < cb->size; i++) {
			newbuf[i] = cb->buffer[i];
		}
		cb->size = 2 * cb->size;

		free(cb->buffer);
		cb->buffer = newbuf;
	}
}

// Destroy the buffer.
void freebuf(compressedBuffer *cb) {
	if (cb != NULL) {
		free(cb->buffer);
		cb->buffer = NULL;
	}
	cb->size = 0;
	cb->pos = 0;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

struct MaskEntry {
	unsigned int mask1;
	unsigned int mask2;
	unsigned int mask3;
	unsigned int mask4;
};

struct LengthEntry {
	unsigned int len1;
	unsigned int len2;
	unsigned int len3;
	unsigned int len4;
};

struct MaskEntry MASKS[256];
struct LengthEntry LENGTHS[256];

void FillTables () {
	int i = 0;
	int b1, b2, b3, b4;

	for (i = 0; i < 256; i++) {
		b4 = (i & 0x3) + 1;
		b3 = ((i >> 2) & 0x3) + 1;
		b2 = ((i >> 4) & 0x3) + 1;
		b1 = ((i >> 6) & 0x3) + 1;

		LENGTHS[i].len1 = b1;
		MASKS[i].mask1 = pow(256, b1) - 1;

		LENGTHS[i].len2 = b2;
		MASKS[i].mask2 = pow(256, b2) - 1;

		LENGTHS[i].len3 = b3;
		MASKS[i].mask3 = pow(256, b3) - 1;

		LENGTHS[i].len4 = b4;
		MASKS[i].mask4 = pow(256, b4) - 1;
	}
}


int PutGVInt(compressedBuffer *cb, unsigned int *n) {
	int ret = 0, id = 0, b = 0, sft = 6, s = cb->pos, val = 0;
	char tag = 0;

	if (n[0] >= 0xFFFFFFFF || n[1] >= 0xFFFFFFFF || n[2] >= 0xFFFFFFFF || n[3] >= 0xFFFFFFFF) {
		printf("Compression general Error. Cannot encode integers greater than 2E32-1\n");
		exit(1);
	}

	cb->pos++;
	if(cb->pos >= cb->size) { doublebuf(cb); }

	for (id = 0; id < 4; id++) {
		b = 0;
		#ifdef DEBUG
		printf("Bytes for number %d: %d\n", id + 1, n[id]);
		#endif

		val = n[id];
		while (val >= 256) {
			if(cb->pos >= cb->size) { doublebuf(cb); }

			cb->buffer[cb->pos++] = val & 0xFF;
			#ifdef DEBUG
			DisplayByte(cb->buffer[cb->pos-1]);
			#endif
			val >>= 8;
			ret++;
			b++;
		}
		if(cb->pos >= cb->size) { doublebuf(cb); }
		cb->buffer[cb->pos++] = val & 0xFF;
		ret++;
		#ifdef DEBUG
		DisplayByte(cb->buffer[cb->pos-1]);
		#endif
		tag |= b << sft;
		sft -= 2;
	}
	#ifdef DEBUG
	printf("\n\nTAGS: ");
	DisplayByte(tag);
	printf("\n\n");
	getchar();
	#endif

	cb->buffer[s] = tag;
	return ret;
}

#define GET_GVINT (cmpr_ptr, dcmpr_ptr); { 						\
	unsigned int tag = cmpr_ptr[0]; 							\
	cmpr_ptr++; 												\
	*dcmpr_ptr = (unsigned int)*((unsigned int *)(cmpr_ptr)); 	\
	*dcmpr_ptr &= MASKS[tag].mask1; 							\
	dcmpr_ptr++; 												\
	cmpr_ptr += LENGTHS[tag].len1; 								\
	*dcmpr_ptr = (unsigned int)*((unsigned int *)(cmpr_ptr)); 	\
	*dcmpr_ptr &= MASKS[tag].mask2; 							\
	dcmpr_ptr++; 												\
	cmpr_ptr += LENGTHS[tag].len2; 								\
	*dcmpr_ptr = (unsigned int)*((unsigned int *)(cmpr_ptr)); 	\
	*dcmpr_ptr &= MASKS[tag].mask3; 							\
	dcmpr_ptr++; 												\
	cmpr_ptr += LENGTHS[tag].len3; 								\
	*dcmpr_ptr = (unsigned int)*((unsigned int *)(cmpr_ptr)); 	\
	*dcmpr_ptr &= MASKS[tag].mask4; 							\
	dcmpr_ptr++;												\
	cmpr_ptr += LENGTHS[tag].len4; 								\
}

void CheckGVInt() {

	int ar_size = 800000000, LEN = 10000000;
	int i, val;
	unsigned int *in = (unsigned int*)malloc(ar_size * sizeof(unsigned int));
	unsigned int *out = (unsigned int*)malloc(ar_size * sizeof(unsigned int));
	unsigned int *in_ptr;
	compressedBuffer cbuf;
	struct timespec et, st, et2, st2;

////////////////// PREPARATION PHASE
	printf("Preparing...\n"); fflush(NULL);

	memset(in, 0, ar_size * sizeof(unsigned int));
	memset(out, 0, ar_size * sizeof(unsigned int));

	initbuf(&cbuf, LEN);
	for (i = 0; i < ar_size; i++) {
		in[i] = (rand () % 256) + 256;
	}

	FillTables();
/*	for (i = 0 ; i < 256; i++) {
		printf("Num1 Mask: %u, Offset:%u\n", tab[i].mask1, tab[i].off1);
		printf("Num2 Mask: %u, Offset:%u\n", tab[i].mask2, tab[i].off2);
		printf("Num3 Mask: %u, Offset:%u\n", tab[i].mask3, tab[i].off3);
		printf("Num4 Mask: %u, Offset:%u\n", tab[i].mask4, tab[i].off4);
		printf("\n");
	}
*/

////////////////// COMPRESSION PHASE

	printf("Compressing integers with Group VarInt...\n"); fflush(NULL);
	clock_gettime(CLOCK_REALTIME, &st);

	in_ptr = in;
	int cmpr_size = 0;
	for (i = 0; i < ar_size; i += 4) {
//		encode4(i, i + 1, i +2, i+3);	
		PutGVInt(&cbuf, in_ptr);
		in_ptr += 4;
	}
	clock_gettime(CLOCK_REALTIME, &et);
	double time = (et.tv_sec - st.tv_sec) + (double)(et.tv_nsec - st.tv_nsec) / (double)BILLION;
	printf ("Compression finished in %6.3f sec. Rate %6.2f million ints/sec.\n", 
		time, (double)ar_size / (time * MILLION));
	printf("Uncompressed Size: %6.2f, Compressed Size: %6.2f MBytes.\n\n",
		(double)ar_size * 4 / (double)MILLION, (double)cbuf.pos / (double)MILLION);

//////////////////////////////////////////////////////////////////
	resetbuf(&cbuf);
//////////////////////////////////////////////////////////////////

	printf("Decompressing integers with Group VarInt...\n"); fflush(NULL);
	unsigned int val1 = 0, val2 = 0, val3 = 0, val4 = 0;
	clock_gettime(CLOCK_REALTIME, &st2);
//	i = 0;
//	int ptr = 0;
//	out_ptr = out;
	unsigned char *cmpr_ptr = cbuf.buffer;
	unsigned int *dcmpr_ptr = out;
	unsigned int id = 0;

	int off, ptr = 0, pp = 0;
	for (i = 0; i < ar_size; i += 4) {

		GET_GVINT(cmpr_ptr, dcmpr_ptr);
		#ifdef DEBUG
		printf("Values: %d, %d, %d, %d\nOffset: %d\n",
			out[id], out[id+1], out[id+2], out[id+3], off);
		getchar();
		#endif

		#ifdef CHECK
		if (val1 != in[pp]) { printf("Error at number %d: It should be %d, but is is %d.", pp, in[pp], val1); exit(1); }
		if (val1 != in[pp+1]) { printf("Error at number %d: It should be %d, but is is %d.", pp+1, in[pp+1], val2); exit(1); }
		if (val1 != in[pp+2]) { printf("Error at number %d: It should be %d, but is is %d.", pp+2, in[pp+2], val3); exit(1); }
		if (val1 != in[pp+3]) { printf("Error at number %d: It should be %d, but is is %d.", pp+3, in[pp+3], val4); exit(1); }
		pp += 4;
		#endif

//		cmpr_ptr += off;
		id+=4;
	}

	clock_gettime(CLOCK_REALTIME, &et2);
	double time2 = (et2.tv_sec - st2.tv_sec) + (double)(et2.tv_nsec - st2.tv_nsec) / (double)BILLION;
	printf ("Decompression finished in %6.3f sec. Rate %6.2f million ints/sec.\n",
		time2, (double)ar_size / (time2 * MILLION));

	for (i = 0; i < ar_size; i++) {
		if (out[i] != in[i]) {
			printf("Error! Number:%d. Decompressed value is :%d, but should be %d.\nExiting...\n\n",
				i, out[i], in[i]);
			exit(1);
		}
	}

	printf("DECOMPRESSION OK!!\n");
	freebuf(&cbuf);
	free(in);
	free(out);
}

