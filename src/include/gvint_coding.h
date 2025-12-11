#include "coding.h"

class gvint_coding : public coding {
public:
	gvint_coding();
	//virtual ~gvint_coding();
	virtual int Compression(unsigned*, unsigned*, int);
	virtual int Decompression(unsigned*, unsigned*, int);
	virtual int get_type();
	virtual void set_size(int size);

private:
	int type;
	int size;
//	int GV_MASKS[4];

	struct MaskEntry {
		unsigned int mask1;
		unsigned int mask2;
		unsigned int mask3;
		unsigned int mask4;
	} GV_MASKS[256];

	struct LengthEntry {
		unsigned int len1;
		unsigned int len2;
		unsigned int len3;
		unsigned int len4;
		unsigned int tot_len;
	} GV_LENGTHS[256];

	unsigned char* tmp;
	int gvint_decoding(unsigned char *input, unsigned * output);
};

void DisplayByte(unsigned int u) {
	int t;

	for(t=128; t>0; t = t/2)
		if(u & t) printf("1 ");
		else printf("0 ");

}

gvint_coding::gvint_coding() {

	int i = 0;
	int b1, b2, b3, b4;

	for (i = 0; i < 256; i++) {
		b4 = (i & 0x3) + 1;
		b3 = ((i >> 2) & 0x3) + 1;
		b2 = ((i >> 4) & 0x3) + 1;
		b1 = ((i >> 6) & 0x3) + 1;

		GV_LENGTHS[i].len1 = b1;
		GV_MASKS[i].mask1 = pow(256, b1) - 1;

		GV_LENGTHS[i].len2 = b2;
		GV_MASKS[i].mask2 = pow(256, b2) - 1;

		GV_LENGTHS[i].len3 = b3;
		GV_MASKS[i].mask3 = pow(256, b3) - 1;

		GV_LENGTHS[i].len4 = b4;
		GV_MASKS[i].mask4 = pow(256, b4) - 1;

		GV_LENGTHS[i].tot_len = 1 + b1 + b2 + b3 + b4;
	}

//	GV_MASKS = { 0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF };
	type = 6;
}

void gvint_coding::set_size(int s) {
	size = s;
	return;
}

int gvint_coding::get_type() {
	return type;
}

int gvint_coding::Compression(unsigned* input, unsigned* output, int size) {
	int i = 0, j = 0, ret = 0, pos = 1, val = 0, b = 0, sft = 6, tagpos = 0;

	char tag = 0;
	char *tmp = (char*)malloc(size * sizeof(unsigned));
	memset(tmp, 0, size * sizeof(unsigned));

	for (i = 0; i < size; i += 4) {
		tagpos = ret;
		sft = 6;
		b = 0;
		tag = 0;

		for (j = 0; j < 4; j++) {
			b = 0;
			val = input[j + i];
//			printf("Value is %d\n", val);
			while (val >= 256) {
				tmp[pos++] = val & 0xFF;
				val >>= 8;
//				DisplayByte(tmp[pos-1]);
				ret++;
				b++;
			}
			tmp[pos++] = val & 0xFF;
			ret++;
//			DisplayByte(tmp[pos-1]);
			tag |= b << sft;
			sft -= 2;
		}

		tmp[tagpos] = tag;

//		printf("Tag: ");
//		DisplayByte(tag);

		ret++;
		pos++;
//		printf("pos:%d, ret: %d, tagpos: %d\n", pos, ret, tagpos);
	}

	memcpy(output, tmp, ret * sizeof(char));
/*
	for (i = 0; i < ret; i++) {
		printf ("Byte %d Content\t", i);
		DisplayByte(tmp[i]);
	}
*/
	free(tmp);

//	if (ret % sizeof(unsigned) == 0) {
//		return ret / sizeof(unsigned);
//	} else {
		return ret / sizeof(unsigned) + 1;
//	}

}



int gvint_coding::Decompression(unsigned* input, unsigned* output, int size) {
	int i = 0;
	int ret = 0, pos = 0;

	unsigned char *tmp = (unsigned char*)input;

//printf("Size=%d\n", size);
	for (i = 0; i < size; i += 4) {
//		printf("Pass %d\n", i + 1);
//		for (int j = 0; j < 9; j++) {
//			DisplayByte(tmp[j]);
//		}

		pos = gvint_decoding(tmp, output);
		tmp += pos;
		output += 4;
		ret += pos;
//		printf("pos %d, ret %d\n", pos, ret);
	}

//	if (ret % sizeof(unsigned) == 0) {
//		return ret / sizeof(unsigned);
//	} else {
		return ret / 4 + 1;
//	}

}

int gvint_coding::gvint_decoding(unsigned char* cmpr_ptr, unsigned* output) {

	unsigned int tag = cmpr_ptr[0];

	cmpr_ptr++;
	*output = (unsigned int)*((unsigned int *)(cmpr_ptr));
	*output &= GV_MASKS[tag].mask1;
	output++;

	cmpr_ptr += GV_LENGTHS[tag].len1;
	*output = (unsigned int)*((unsigned int *)(cmpr_ptr));
	*output &= GV_MASKS[tag].mask2;
	output++;

	cmpr_ptr += GV_LENGTHS[tag].len2;
	*output = (unsigned int)*((unsigned int *)(cmpr_ptr));
	*output &= GV_MASKS[tag].mask3;
	output++;

	cmpr_ptr += GV_LENGTHS[tag].len3;
	*output = (unsigned int)*((unsigned int *)(cmpr_ptr));
	*output &= GV_MASKS[tag].mask4;
	output++;
	cmpr_ptr += GV_LENGTHS[tag].len4;

	return GV_LENGTHS[tag].tot_len;
}


/*
void DecodeGroupVarInt(const byte* compressed, int size, uint32_t* uncompressed) {
  const uint32_t MASK[4] = { 0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF };
  const byte* limit = compressed + size;
  uint32_t current_value = 0;
  while (compressed != limit) {
    const uint32_t selector = *compressed++;
    const uint32_t selector1 = (selector & 3);
    current_value += *((uint32_t*)(compressed)) & MASK[selector1];
    *uncompressed++ = current_value;
    compressed += selector1 + 1;
    const uint32_t selector2 = ((selector >> 2) & 3);
    current_value += *((uint32_t*)(compressed)) & MASK[selector2];
    *uncompressed++ = current_value;
    compressed += selector2 + 1;
    const uint32_t selector3 = ((selector >> 4) & 3);
    current_value += *((uint32_t*)(compressed)) & MASK[selector3];
    *uncompressed++ = current_value;
    compressed += selector3 + 1;
    const uint32_t selector4 = (selector >> 6);
    current_value += *((uint32_t*)(compressed)) & MASK[selector4];
    *uncompressed++ = current_value;
    compressed += selector4 + 1;
  }
}
*/
