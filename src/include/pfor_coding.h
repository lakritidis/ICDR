#ifndef PFOR_CODING_H_
#define PFOR_CODING_H_


#ifndef TEST
#define TEST 0
#endif

class pfor_coding : public coding
{
public:
	pfor_coding();

	virtual int Compression(unsigned*, unsigned*, int);
	int Compression(unsigned* in, unsigned* );
	int Decompression(unsigned* in, unsigned*);
	virtual int Decompression(unsigned*, unsigned*, int);
	//virtual ~pfor_coding();
	virtual int get_type();
	virtual void set_size(int size);
private:
	int pfor_encode(unsigned int **w, unsigned int *p, int num);
	unsigned* pfor_decode(unsigned int *_p, unsigned int *_w, int flag);
	//void pack(unsigned int *v, unsigned int b, unsigned int n, unsigned int *w);
	int Bs;
	int coding_type;

	//is the percentage of exceptions in the array. default value is 0.1
	float FRAC;
	int cnum[17];

	/***************************************
	 * meta data in pfordelta
	 **************************************/
	int b;
	int start;
	int t;
	int unpack_count;
};

pfor_coding::pfor_coding():FRAC(0.1)
{
	coding_type = 1;
	cnum[0] = 0;
	cnum[1] = 1;
	cnum[2] = 2;
	cnum[3] = 3;
	cnum[4] = 4;
	cnum[5] = 5;
	cnum[6] = 6;
	cnum[7] = 7;
	cnum[8] = 8;
	cnum[9] = 9;
	cnum[10] = 10;
	cnum[11] = 11;
	cnum[12] = 12;
	cnum[13] = 13;
	cnum[14] = 16;
	cnum[15] = 20;
	cnum[16] = 32;

	//set default block_size as
	Bs = 128;
}

int pfor_coding::get_type() {
	return coding_type;
}

void pfor_coding::set_size(int size) {
	/*for pfordelta we use static block size*/
	if (size < 64) {
		Bs = 32;
	} else if (size < 128) {
		Bs = 64;
	} else if (size < 256) {
		Bs = 128;
	} else {
		Bs = 256;
	}
}


int pfor_coding::Compression(unsigned* input, unsigned* output) {
	int flag = -1;
	unsigned* w;
	for (int k = 0; flag < 0; k++) {
		w = output+1;
		flag = pfor_encode(&w, input, k);
	}

	*output = flag;
	return w - output;
}


int pfor_coding::Compression(unsigned* input, unsigned* output, int s) {
	return Compression(input, output);
}


int pfor_coding::Decompression(unsigned* input, unsigned* output, int s) {
	return Decompression(input, output);
}


int pfor_coding::Decompression(unsigned* input, unsigned* output){
	unsigned* tmp = input;
	int flag = (*tmp);
	b = cnum[((flag>>12) & 15)+2];
	unpack_count = ((flag>>12) & 15)+2;
	//cout << "upack cout:"<<unpack_count<<endl;
	t = (flag>>10) & 3;
	start = flag & 1023;

	tmp++;
	tmp = pfor_decode(output, tmp, flag);
	return tmp - input;

}

int pfor_coding::pfor_encode(unsigned int **w, unsigned int *p, int num)
{
	int i, l, n, bb, t, s;
	unsigned int m;
	int b = cnum[num+2];
	int start;

	unsigned int out[Bs];
	unsigned int ex[Bs];

	if (b == 32)
	{
		for (i = 0; i < Bs; i++)  (*w)[i] = p[i];
		*w += Bs;
		return ((num<<12) + (2<<10) + Bs);
	}

	for (m = 0, i = 0; i < Bs; i++)
		if (p[i] > m)
			m = p[i];

	if (m < 256)
	{
		bb = 8;
		t = 0;
	}

	else if (m < 65536)
	{
		bb = 16;
		t = 1;
	}
	else
	{
		bb = 32;
		t = 2;
	}

	for (start = 0, n = 0, l = -1, i = 0; i < Bs; i++)
	{
		if ((p[i] >= (1 << b)) || ((l >= 0) && (i-l == (1<<b))))
		{
			if (l < 0)
				start = i;
			else
				out[l] = i - l - 1;

			ex[n++] = p[i];
			l = i;
		}
		else
			out[i] = p[i];
	}

	if (l >= 0)
		out[l] = (1<<b)-1;
	else
		start = Bs;

	if ((double)(n) <= FRAC * (double)(Bs))
	{
		s = ((b * Bs)>>5);
		for (i = 0; i < s; i++)
			(*w)[i] = 0;
		pack(out, b, Bs, *w);
		*w += s;

		s = ((bb * n)>>5) + ((((bb * n) & 31) > 0)? 1:0);
		for (i = 0; i < s; i++)
			(*w)[i] = 0;
		pack(ex, bb, n, *w);
		*w += s;
		return ((num<<12) + (t<<10) + start);
	}
	else
		return(-1);
}


unsigned* pfor_coding::pfor_decode(unsigned int *_p, unsigned int *_w, int flag)
{
	int i, s;
	unsigned int x;
	(unpack[unpack_count])(_p, _w, Bs);
	_w += ((b * Bs)>>5);

	switch (t)
	{
		case(0):
			for (s = start, i = 0; s < Bs; i++)
			{
				x = _p[s]+1;
				_p[s] = (_w[i>>2]>>(24-((i&3)<<3)))&255;
				s += x;
			}
			_w += (i>>2);

			if ((i&3) > 0)
				_w++;
			break;

		case(1):
			for (s = start, i = 0; s < Bs; i++)
			{
				x = _p[s]+1;
				_p[s] = (_w[i>>1]>>(16-((i&1)<<4)))&65535;
				s += x;
			}
			_w += (i>>1);
			if ((i&1) > 0)  _w++;
			break;

		case(2):
			for (s = start, i=0; s < Bs; i++)
			{
				x = _p[s]+1;
				_p[s] = _w[i];
				s += x;
			}
			_w += i;
			break;
	}
	return(_w);
}

#endif //PFOR_CODING_H_
