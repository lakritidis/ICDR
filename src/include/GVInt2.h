static inline unsigned int GetNum(unsigned char *vp, int n, int *pos) {
	unsigned int r;
	#define next_byte (unsigned int)(vp[(*pos)++] & 0xFF)

	switch (n) {
		case 0x0:
			r = next_byte;
			break;
		case 0x1:
			r = next_byte;
			r |= next_byte << 8;
			break;
		case 0x2:
			r = next_byte;
			r |= next_byte << 8;
			r |= next_byte << 16;
			break;
		case 0x3:
			r = next_byte;
			r |= next_byte << 8;
			r |= next_byte << 16;
			r |= next_byte << 24;
			break;
	}
	#undef next_byte
	return r;
}

int GetGVInt(unsigned char *vp, unsigned int *val1, unsigned int *val2, unsigned int *val3, unsigned int *val4) {

	unsigned int tag = vp[0];
//	DisplayByte(vp[0]);
	int ret = 1, bytes = 0, start = 1;

	bytes = (tag >> 6) & 0x3;
	ret = ret + bytes + 1;
	*val1 = GetNum(vp, bytes, &start);
//	printf("Bytes: %d. Start: %d. Return: %d\n", bytes, start, ret);

	bytes = (tag >> 4) & 0x3;
	ret = ret + bytes + 1;
	*val2 = GetNum(vp, bytes, &start);
//printf("Bytes: %d. Start: %d. Return: %d\n", bytes, start, ret);

	bytes = (tag >> 2) & 0x3;
	ret = ret + bytes + 1;
	*val3 = GetNum(vp, bytes, &start);
//printf("Bytes: %d. Start: %d. Return: %d\n", bytes, start, ret);

	bytes = (tag >> 0) & 0x3;
	ret = ret + bytes + 1;
	*val4 = GetNum(vp, bytes, &start);
//printf("Bytes: %d. Start: %d. Return: %d\n", bytes, start, ret);
	return ret;
}

int GetGVInt2(BITVEC *vp, unsigned int *val1, unsigned int *val2, unsigned int *val3, unsigned int *val4) {

	#define next_byte (unsigned int)vp->vector[vp->pos++]

	int tag = next_byte;
	*val1 = 0;
	*val2 = 0;
	*val3 = 0;
	*val4 = 0;

	switch (((tag >> 6) & 0x3) + 1) {
		case 1:
			*val1 = next_byte;
			break;

		case 2:
			*val1 |= next_byte;
			*val1 |= next_byte << 8;
			break;

		case 3:
			*val1 |= next_byte;
			*val1 |= next_byte << 8;
			*val1 |= next_byte << 16;
			break;

		case 4:
			*val1 |= next_byte;
			*val1 |= next_byte << 8;
			*val1 |= next_byte << 16;
			*val1 |= next_byte << 24;
			break;
	}

	switch (((tag >> 4) & 0x3) + 1) {
		case 1:
			*val2 = next_byte;
			break;

		case 2:
			*val2 |= next_byte;
			*val2 |= next_byte << 8;
			break;

		case 3:
			*val2 |= next_byte;
			*val2 |= next_byte << 8;
			*val2 |= next_byte << 16;
			break;

		case 4:
			*val2 |= next_byte;
			*val2 |= next_byte << 8;
			*val2 |= next_byte << 16;
			*val2 |= next_byte << 24;
			break;
	}

	switch (((tag >> 2) & 0x3) + 1) {
		case 1:
			*val3 = next_byte;
			break;

		case 2:
			*val3 |= next_byte;
			*val3 |= next_byte << 8;
			break;

		case 3:
			*val3 |= next_byte;
			*val3 |= next_byte << 8;
			*val3 |= next_byte << 16;
			break;

		case 4:
			*val3 |= next_byte;
			*val3 |= next_byte << 8;
			*val3 |= next_byte << 16;
			*val3 |= next_byte << 24;
			break;
	}

	switch ((tag & 0x3) + 1) {
		case 1:
			*val4 = next_byte;
			break;

		case 2:
			*val4 |= next_byte;
			*val4 |= next_byte << 8;
			break;

		case 3:
			*val4 |= next_byte;
			*val4 |= next_byte << 8;
			*val4 |= next_byte << 16;
			break;

		case 4:
			*val4 |= next_byte;
			*val4 |= next_byte << 8;
			*val4 |= next_byte << 16;
			*val4 |= next_byte << 24;
			break;
	}

	#undef next_byte
}

void encode4(unsigned int x0, unsigned int x1, unsigned int x2, unsigned int x3) {
	# define L(x) ((x >> 8) == 0ull ? 0 : \
		     (x >> 16) == 0ull ? 1 : \
		     (x >> 32) == 0ull ? 2 : 3)
	# define P(l, x) l = 1 << l;\
		do putchar((unsigned char)(x)),x >>= 8; while (0 != --l)
	unsigned char l0 = L(x0);
	unsigned char l1 = L(x1);
	unsigned char l2 = L(x2);
	unsigned char l3 = L(x3);

	/* Write four lengths as one byte */
	putchar((((((l0<<2)|l1)<<2)|l2)<<2)|l3);

	/* Write each number as 1..4 bytes */
	P(l0, x0);
	P(l1, x1);
	P(l2, x2);
	P(l3, x3);
	# undef P
	# undef L
}

static inline unsigned int get_1_to_8(int n) {
	unsigned int r;
    # define next_byte (unsigned int)getchar()

	r = next_byte;		/* placed here to shut gcc up */
	switch (n&3) {
	    case 0:
		break;
	    case 1:
		r |= next_byte << 8;
		break;
	    case 2:
		r |= next_byte <<  8;
		r |= next_byte << 16;
		r |= next_byte << 24;
		break;
	    case 3:
		r |= next_byte <<  8;
		r |= next_byte << 16;
		r |= next_byte << 24;
		r |= next_byte << 32;
		r |= next_byte << 40;
		r |= next_byte << 48;
		r |= next_byte << 56;
		break;
	}
	return r;
      # undef next_byte
}

void decode4(unsigned int *x0, unsigned int *x1, unsigned int *x2, unsigned int *x3) {
	unsigned char c = getchar();
	*x0 = get_1_to_8(c >> 6);
	*x1 = get_1_to_8(c >> 4);
	*x2 = get_1_to_8(c >> 2);
	*x3 = get_1_to_8(c >> 0);
}
