#ifndef _CODING_FACTORY_H_
#define _CODING_FACTORY_H_

#include "coding.h"
#include "rice_coding.h"
#include "pfor_coding.h"
#include "tzp_coding.h"
#include "vbyte_coding.h"
#include "s9_coding.h"
//#include "s16_coding.h"
//#include "rice_coding2.h"


#define RICE 0
#define PFOR 1
#define VBYT 2
#define TRICE 3
#define S9 4
#define S16 5

class coding_factory {
public:
	coding_factory();
	coding* get_coder(int type);
	~coding_factory();
};


#endif
