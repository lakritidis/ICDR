#ifndef VBYTE_CODING_H_
#define VBYTE_CODING_H_

#include "coding.h"

class vbyte_coding : public coding
{
public:
	vbyte_coding();
	//virtual ~vbyte_coding();
	virtual int Compression(unsigned*, unsigned*, int);
	virtual int Decompression(unsigned*, unsigned*, int);
	virtual int get_type();
	virtual void set_size(int size);

private:
	int type;
	int size;
	unsigned char* tmp;
	inline void vbencoding(unsigned);
	inline unsigned vbdecoding(unsigned char*);
};

#endif //VBYTE_CODING_H_

vbyte_coding::vbyte_coding()
{
	type = 2;
}



void vbyte_coding::set_size(int s)
{
	return;
}

int vbyte_coding::get_type()
{
	return type;
}

unsigned vbyte_coding::vbdecoding(unsigned char* _v)
{
	unsigned _n = ((*tmp>>1));
	if ((*tmp&0x1) != 0)
	{
		tmp++;
		_n = (_n << 7) | ((*tmp >> 1));
		if ((*tmp&0x1)!= 0)
		{
			tmp++;
			_n=(_n<<7)|(((*tmp)>>1));
			if ((*tmp&0x1) != 0)
			{
				tmp++;
				_n = (_n<<7) | ((*tmp>>1));
			}
		}
	}
	tmp++;
	return _n;
}

void vbyte_coding::vbencoding(unsigned int _n) {
	unsigned _num;
	unsigned char _barray[5];
	unsigned _i, _started = 0;

	_num = _n;
	for (_i = 0; _i < 5; _i++) {
		_barray[_i] = (_num & 0x07F) << 1;
		_num = _num >> 7 & 0x0FFFFFFF;
	}

	for (_i = 4; _i > 0; _i--) {
		if (_barray[_i] != 0 || _started == 1) {
			_started = 1;
			*tmp = _barray[_i] | 0x1;
			tmp++;
		}
	}

	*tmp = _barray[0] | 0x0;
	tmp++;
}

int vbyte_coding::Compression(unsigned int * input, unsigned int * output, int size) {
	int i;
	tmp = (unsigned char *)output;

	for (i = 0; i < size; i++) {
		vbencoding(input[i]);
	}
	//cout << ((unsigned int*)tmp) - ((unsigned*)output)<<endl;
	return (((unsigned int*)tmp) - ((unsigned*)output))+1;
}

int vbyte_coding::Decompression(unsigned* input, unsigned* output, int size) {
	int i;
	tmp = (unsigned char*)input;
	for (i = 0; i < size; i++) {
		output[i] = vbdecoding(tmp);
		//cout << output[i] << endl;
	}

	return (((unsigned int*)tmp) - ((unsigned int*)input))+1;
}


