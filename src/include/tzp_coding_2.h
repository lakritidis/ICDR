/// ///////////////////////////////////////////////////////////////////////////////////////////////
/// HEADIR Large Scale Retrieval System
///
/// TZP coder descriptor (encode/decode)
///
/// Leonidas Akritidis, November 7th, 2011
/// ///////////////////////////////////////////////////////////////////////////////////////////////

#include "coding.h"

typedef void (*bit_unpacker)(uint32_t *i, uint32_t *o);

/// Get 1 bit functions
void get_1bits_start00 (uint32_t* i, uint32_t *o) { *o = i[0] >> 31; }
void get_1bits_start01 (uint32_t* i, uint32_t *o) { *o = (i[0] >> 30) & 1; }
void get_1bits_start02 (uint32_t* i, uint32_t *o) { *o = (i[0] >> 29) & 1; }
void get_1bits_start03 (uint32_t* i, uint32_t *o) { *o = (i[0] >> 28) & 1; }
void get_1bits_start04 (uint32_t* i, uint32_t *o) { *o = (i[0] >> 27) & 1; }
void get_1bits_start05 (uint32_t* i, uint32_t *o) { *o = (i[0] >> 26) & 1; }
void get_1bits_start06 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 25) & 1; }
void get_1bits_start07 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 24) & 1; }
void get_1bits_start08 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 23) & 1; }
void get_1bits_start09 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 22) & 1; }
void get_1bits_start10 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 21) & 1; }
void get_1bits_start11 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 20) & 1; }
void get_1bits_start12 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 19) & 1; }
void get_1bits_start13 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 18) & 1; }
void get_1bits_start14 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 17) & 1; }
void get_1bits_start15 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 16) & 1; }
void get_1bits_start16 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 15) & 1; }
void get_1bits_start17 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 14) & 1; }
void get_1bits_start18 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 13) & 1; }
void get_1bits_start19 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 12) & 1; }
void get_1bits_start20 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 11) & 1; }
void get_1bits_start21 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 10) & 1; }
void get_1bits_start22 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 9) & 1; }
void get_1bits_start23 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 8) & 1; }
void get_1bits_start24 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 7) & 1; }
void get_1bits_start25 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 6) & 1; }
void get_1bits_start26 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 5) & 1; }
void get_1bits_start27 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 4) & 1; }
void get_1bits_start28 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 3) & 1; }
void get_1bits_start29 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 2) & 1; }
void get_1bits_start30 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 1) & 1; }
void get_1bits_start31 (uint32_t* i, uint32_t* o) { *o = i[0] & 1; }

/// Get 2 bits functions
void get_2bits_start00 (uint32_t* i, uint32_t* o) { *o = i[0] >> 30; }
void get_2bits_start01 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 29) & 3; }
void get_2bits_start02 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 28) & 3; }
void get_2bits_start03 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 27) & 3; }
void get_2bits_start04 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 26) & 3; }
void get_2bits_start05 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 25) & 3; }
void get_2bits_start06 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 24) & 3; }
void get_2bits_start07 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 23) & 3; }
void get_2bits_start08 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 22) & 3; }
void get_2bits_start09 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 21) & 3; }
void get_2bits_start10 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 20) & 3; }
void get_2bits_start11 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 19) & 3; }
void get_2bits_start12 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 18) & 3; }
void get_2bits_start13 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 17) & 3; }
void get_2bits_start14 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 16) & 3; }
void get_2bits_start15 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 15) & 3; }
void get_2bits_start16 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 14) & 3; }
void get_2bits_start17 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 13) & 3; }
void get_2bits_start18 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 12) & 3; }
void get_2bits_start19 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 11) & 3; }
void get_2bits_start20 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 10) & 3; }
void get_2bits_start21 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 9) & 3; }
void get_2bits_start22 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 8) & 3; }
void get_2bits_start23 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 7) & 3; }
void get_2bits_start24 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 6) & 3; }
void get_2bits_start25 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 5) & 3; }
void get_2bits_start26 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 4) & 3; }
void get_2bits_start27 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 3) & 3; }
void get_2bits_start28 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 2) & 3; }
void get_2bits_start29 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 1) & 3; }
void get_2bits_start30 (uint32_t* i, uint32_t* o) { *o = i[0] & 3; }
void get_2bits_start31 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 1) & 3) | (i[1] >> 31); }

/// Get 3 bits functions
void get_3bits_start00 (uint32_t* i, uint32_t* o) { *o = i[0] >> 29; }
void get_3bits_start01 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 28) & 7; }
void get_3bits_start02 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 27) & 7; }
void get_3bits_start03 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 26) & 7; }
void get_3bits_start04 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 25) & 7; }
void get_3bits_start05 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 24) & 7; }
void get_3bits_start06 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 23) & 7; }
void get_3bits_start07 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 22) & 7; }
void get_3bits_start08 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 21) & 7; }
void get_3bits_start09 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 20) & 7; }
void get_3bits_start10 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 19) & 7; }
void get_3bits_start11 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 18) & 7; }
void get_3bits_start12 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 17) & 7; }
void get_3bits_start13 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 16) & 7; }
void get_3bits_start14 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 15) & 7; }
void get_3bits_start15 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 14) & 7; }
void get_3bits_start16 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 13) & 7; }
void get_3bits_start17 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 12) & 7; }
void get_3bits_start18 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 11) & 7; }
void get_3bits_start19 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 10) & 7; }
void get_3bits_start20 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 9) & 7; }
void get_3bits_start21 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 8) & 7; }
void get_3bits_start22 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 7) & 7; }
void get_3bits_start23 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 6) & 7; }
void get_3bits_start24 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 5) & 7; }
void get_3bits_start25 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 4) & 7; }
void get_3bits_start26 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 3) & 7; }
void get_3bits_start27 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 2) & 7; }
void get_3bits_start28 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 1) & 7; }
void get_3bits_start29 (uint32_t* i, uint32_t* o) { *o = i[0] & 7; }
void get_3bits_start30 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 1) & 7) | (i[1] >> 31); }
void get_3bits_start31 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 2) & 7) | ((i[1] >> 30) & 3); }

/// Get 4 bits functions
void get_4bits_start00 (uint32_t* i, uint32_t* o) { *o = i[0] >> 28; }
void get_4bits_start01 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 27) & 15; }
void get_4bits_start02 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 26) & 15; }
void get_4bits_start03 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 25) & 15; }
void get_4bits_start04 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 24) & 15; }
void get_4bits_start05 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 23) & 15; }
void get_4bits_start06 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 22) & 15; }
void get_4bits_start07 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 21) & 15; }
void get_4bits_start08 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 20) & 15; }
void get_4bits_start09 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 19) & 15; }
void get_4bits_start10 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 18) & 15; }
void get_4bits_start11 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 17) & 15; }
void get_4bits_start12 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 16) & 15; }
void get_4bits_start13 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 15) & 15; }
void get_4bits_start14 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 14) & 15; }
void get_4bits_start15 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 13) & 15; }
void get_4bits_start16 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 12) & 15; }
void get_4bits_start17 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 11) & 15; }
void get_4bits_start18 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 10) & 15; }
void get_4bits_start19 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 9) & 15; }
void get_4bits_start20 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 8) & 15; }
void get_4bits_start21 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 7) & 15; }
void get_4bits_start22 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 6) & 15; }
void get_4bits_start23 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 5) & 15; }
void get_4bits_start24 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 4) & 15; }
void get_4bits_start25 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 3) & 15; }
void get_4bits_start26 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 2) & 15; }
void get_4bits_start27 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 1) & 15; }
void get_4bits_start28 (uint32_t* i, uint32_t* o) { *o = i[0] & 15; }
void get_4bits_start29 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 1) & 15) | (i[1] >> 31); }
void get_4bits_start30 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 2) & 15) | ((i[1] >> 30) & 3); }
void get_4bits_start31 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 3) & 15) | ((i[1] >> 29) & 7); }

/// Get 5 bits functions
void get_5bits_start00 (uint32_t* i, uint32_t* o) { *o = i[0] >> 27; }
void get_5bits_start01 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 26) & 31; }
void get_5bits_start02 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 25) & 31; }
void get_5bits_start03 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 24) & 31; }
void get_5bits_start04 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 23) & 31; }
void get_5bits_start05 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 22) & 31; }
void get_5bits_start06 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 21) & 31; }
void get_5bits_start07 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 20) & 31; }
void get_5bits_start08 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 19) & 31; }
void get_5bits_start09 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 18) & 31; }
void get_5bits_start10 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 17) & 31; }
void get_5bits_start11 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 16) & 31; }
void get_5bits_start12 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 15) & 31; }
void get_5bits_start13 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 14) & 31; }
void get_5bits_start14 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 13) & 31; }
void get_5bits_start15 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 12) & 31; }
void get_5bits_start16 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 11) & 31; }
void get_5bits_start17 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 10) & 31; }
void get_5bits_start18 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 9) & 31; }
void get_5bits_start19 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 8) & 31; }
void get_5bits_start20 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 7) & 31; }
void get_5bits_start21 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 6) & 31; }
void get_5bits_start22 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 5) & 31; }
void get_5bits_start23 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 4) & 31; }
void get_5bits_start24 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 3) & 31; }
void get_5bits_start25 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 2) & 31; }
void get_5bits_start26 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 1) & 31; }
void get_5bits_start27 (uint32_t* i, uint32_t* o) { *o = i[0] & 31; }
void get_5bits_start28 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 1) & 31) | (i[1] >> 31); }
void get_5bits_start29 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 2) & 31) | ((i[1] >> 30) & 3); }
void get_5bits_start30 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 3) & 31) | ((i[1] >> 29) & 7); }
void get_5bits_start31 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 4) & 31) | ((i[1] >> 28) & 15); }

/// Get 6 bits functions
inline void get_6bits_start00 (uint32_t* i, uint32_t* o) { *o = i[0] >> 26; }
inline void get_6bits_start01 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 25) & 63; }
inline void get_6bits_start02 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 24) & 63; }
inline void get_6bits_start03 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 23) & 63; }
inline void get_6bits_start04 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 22) & 63; }
inline void get_6bits_start05 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 21) & 63; }
inline void get_6bits_start06 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 20) & 63; }
inline void get_6bits_start07 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 19) & 63; }
inline void get_6bits_start08 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 18) & 63; }
inline void get_6bits_start09 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 17) & 63; }
inline void get_6bits_start10 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 16) & 63; }
inline void get_6bits_start11 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 15) & 63; }
inline void get_6bits_start12 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 14) & 63; }
inline void get_6bits_start13 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 13) & 63; }
inline void get_6bits_start14 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 12) & 63; }
inline void get_6bits_start15 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 11) & 63; }
inline void get_6bits_start16 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 10) & 63; }
inline void get_6bits_start17 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 9) & 63; }
inline void get_6bits_start18 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 8) & 63; }
inline void get_6bits_start19 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 7) & 63; }
inline void get_6bits_start20 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 6) & 63; }
inline void get_6bits_start21 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 5) & 63; }
inline void get_6bits_start22 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 4) & 63; }
inline void get_6bits_start23 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 3) & 63; }
inline void get_6bits_start24 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 2) & 63; }
inline void get_6bits_start25 (uint32_t* i, uint32_t* o) { *o = (i[0] >> 1) & 63; }
inline void get_6bits_start26 (uint32_t* i, uint32_t* o) { *o = i[0] & 63; }
inline void get_6bits_start27 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 1) & 63) | (i[1] >> 31); }
inline void get_6bits_start28 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 2) & 63) | ((i[1] >> 30) & 3); }
inline void get_6bits_start29 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 3) & 63) | ((i[1] >> 29) & 7); }
inline void get_6bits_start30 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 4) & 63) | ((i[1] >> 28) & 15); }
inline void get_6bits_start31 (uint32_t* i, uint32_t* o) { *o = ((i[0] << 5) & 63) | ((i[1] >> 27) & 31); }

bit_unpacker TZP_unpacker[] = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	get_1bits_start00, get_1bits_start01, get_1bits_start02, get_1bits_start03,
	get_1bits_start04, get_1bits_start05, get_1bits_start06, get_1bits_start07,
	get_1bits_start08, get_1bits_start09, get_1bits_start10, get_1bits_start11,
	get_1bits_start12, get_1bits_start13, get_1bits_start14, get_1bits_start15,
	get_1bits_start16, get_1bits_start17, get_1bits_start18, get_1bits_start19,
	get_1bits_start20, get_1bits_start21, get_1bits_start22, get_1bits_start23,
	get_1bits_start24, get_1bits_start25, get_1bits_start26, get_1bits_start27,
	get_1bits_start28, get_1bits_start29, get_1bits_start30, get_1bits_start31,

	get_2bits_start00, get_2bits_start01, get_2bits_start02, get_2bits_start03,
	get_2bits_start04, get_2bits_start05, get_2bits_start06, get_2bits_start07,
	get_2bits_start08, get_2bits_start09, get_2bits_start10, get_2bits_start11,
	get_2bits_start12, get_2bits_start13, get_2bits_start14, get_2bits_start15,
	get_2bits_start16, get_2bits_start17, get_2bits_start18, get_2bits_start19,
	get_2bits_start20, get_2bits_start21, get_2bits_start22, get_2bits_start23,
	get_2bits_start24, get_2bits_start25, get_2bits_start26, get_2bits_start27,
	get_2bits_start28, get_2bits_start29, get_2bits_start30, get_2bits_start31,

	get_3bits_start00, get_3bits_start01, get_3bits_start02, get_3bits_start03,
	get_3bits_start04, get_3bits_start05, get_3bits_start06, get_3bits_start07,
	get_3bits_start08, get_3bits_start09, get_3bits_start10, get_3bits_start11,
	get_3bits_start12, get_3bits_start13, get_3bits_start14, get_3bits_start15,
	get_3bits_start16, get_3bits_start17, get_3bits_start18, get_3bits_start19,
	get_3bits_start20, get_3bits_start21, get_3bits_start22, get_3bits_start23,
	get_3bits_start24, get_3bits_start25, get_3bits_start26, get_3bits_start27,
	get_3bits_start28, get_3bits_start29, get_3bits_start30, get_3bits_start31,

	get_4bits_start00, get_4bits_start01, get_4bits_start02, get_4bits_start03,
	get_4bits_start04, get_4bits_start05, get_4bits_start06, get_4bits_start07,
	get_4bits_start08, get_4bits_start09, get_4bits_start10, get_4bits_start11,
	get_4bits_start12, get_4bits_start13, get_4bits_start14, get_4bits_start15,
	get_4bits_start16, get_4bits_start17, get_4bits_start18, get_4bits_start19,
	get_4bits_start20, get_4bits_start21, get_4bits_start22, get_4bits_start23,
	get_4bits_start24, get_4bits_start25, get_4bits_start26, get_4bits_start27,
	get_4bits_start28, get_4bits_start29, get_4bits_start30, get_4bits_start31,

	get_5bits_start00, get_5bits_start01, get_5bits_start02, get_5bits_start03,
	get_5bits_start04, get_5bits_start05, get_5bits_start06, get_5bits_start07,
	get_5bits_start08, get_5bits_start09, get_5bits_start10, get_5bits_start11,
	get_5bits_start12, get_5bits_start13, get_5bits_start14, get_5bits_start15,
	get_5bits_start16, get_5bits_start17, get_5bits_start18, get_5bits_start19,
	get_5bits_start20, get_5bits_start21, get_5bits_start22, get_5bits_start23,
	get_5bits_start24, get_5bits_start25, get_5bits_start26, get_5bits_start27,
	get_5bits_start28, get_5bits_start29, get_5bits_start30, get_5bits_start31,

	get_6bits_start00, get_6bits_start01, get_6bits_start02, get_6bits_start03,
	get_6bits_start04, get_6bits_start05, get_6bits_start06, get_6bits_start07,
	get_6bits_start08, get_6bits_start09, get_6bits_start10, get_6bits_start11,
	get_6bits_start12, get_6bits_start13, get_6bits_start14, get_6bits_start15,
	get_6bits_start16, get_6bits_start17, get_6bits_start18, get_6bits_start19,
	get_6bits_start20, get_6bits_start21, get_6bits_start22, get_6bits_start23,
	get_6bits_start24, get_6bits_start25, get_6bits_start26, get_6bits_start27,
	get_6bits_start28, get_6bits_start29, get_6bits_start30, get_6bits_start31
/*
	get_7bits_start00, get_7bits_start01, get_7bits_start02, get_7bits_start03,
	get_7bits_start04, get_7bits_start05, get_7bits_start06, get_7bits_start07,
	get_7bits_start08, get_7bits_start09, get_7bits_start10, get_7bits_start11,
	get_7bits_start12, get_7bits_start13, get_7bits_start14, get_7bits_start15,
	get_7bits_start16, get_7bits_start17, get_7bits_start18, get_7bits_start19,
	get_7bits_start20, get_7bits_start21, get_7bits_start22, get_7bits_start23,
	get_7bits_start24, get_7bits_start25, get_7bits_start26, get_7bits_start27,
	get_7bits_start28, get_7bits_start29, get_7bits_start30, get_7bits_start31

	get_8bits_start00, get_8bits_start01, get_8bits_start02, get_8bits_start03,
	get_8bits_start04, get_8bits_start05, get_8bits_start06, get_8bits_start07,
	get_8bits_start08, get_8bits_start09, get_8bits_start10, get_8bits_start11,
	get_8bits_start12, get_8bits_start13, get_8bits_start14, get_8bits_start15,
	get_8bits_start16, get_8bits_start17, get_8bits_start18, get_8bits_start19,
	get_8bits_start20, get_8bits_start21, get_8bits_start22, get_8bits_start23,
	get_8bits_start24, get_8bits_start25, get_8bits_start26, get_8bits_start27,
	get_8bits_start28, get_8bits_start29, get_8bits_start30, get_8bits_start31

	get_9bits_start00, get_9bits_start01, get_9bits_start02, get_9bits_start03,
	get_9bits_start04, get_9bits_start05, get_9bits_start06, get_9bits_start07,
	get_9bits_start08, get_9bits_start09, get_9bits_start10, get_9bits_start11,
	get_9bits_start12, get_9bits_start13, get_9bits_start14, get_9bits_start15,
	get_9bits_start16, get_9bits_start17, get_9bits_start18, get_9bits_start19,
	get_9bits_start20, get_9bits_start21, get_9bits_start22, get_9bits_start23,
	get_9bits_start24, get_9bits_start25, get_9bits_start26, get_9bits_start27,
	get_9bits_start28, get_9bits_start29, get_9bits_start30, get_9bits_start31

	get_10bits_start00, get_10bits_start01, get_10bits_start02, get_10bits_start03,
	get_10bits_start04, get_10bits_start05, get_10bits_start06, get_10bits_start07,
	get_10bits_start08, get_10bits_start09, get_10bits_start10, get_10bits_start11,
	get_10bits_start12, get_10bits_start13, get_10bits_start14, get_10bits_start15,
	get_10bits_start16, get_10bits_start17, get_10bits_start18, get_10bits_start19,
	get_10bits_start20, get_10bits_start21, get_10bits_start22, get_10bits_start23,
	get_10bits_start24, get_10bits_start25, get_10bits_start26, get_10bits_start27,
	get_10bits_start28, get_10bits_start29, get_10bits_start30, get_10bits_start31

	get_11bits_start00, get_11bits_start01, get_11bits_start02, get_11bits_start03,
	get_11bits_start04, get_11bits_start05, get_11bits_start06, get_11bits_start07,
	get_11bits_start08, get_11bits_start09, get_11bits_start10, get_11bits_start11,
	get_11bits_start12, get_11bits_start13, get_11bits_start14, get_11bits_start15,
	get_11bits_start16, get_11bits_start17, get_11bits_start18, get_11bits_start19,
	get_11bits_start20, get_11bits_start21, get_11bits_start22, get_11bits_start23,
	get_11bits_start24, get_11bits_start25, get_11bits_start26, get_11bits_start27,
	get_11bits_start28, get_11bits_start29, get_11bits_start30, get_11bits_start31

	get_12bits_start00, get_12bits_start01, get_12bits_start02, get_12bits_start03,
	get_12bits_start04, get_12bits_start05, get_12bits_start06, get_12bits_start07,
	get_12bits_start08, get_12bits_start09, get_12bits_start10, get_12bits_start11,
	get_12bits_start12, get_12bits_start13, get_12bits_start14, get_12bits_start15,
	get_12bits_start16, get_12bits_start17, get_12bits_start18, get_12bits_start19,
	get_12bits_start20, get_12bits_start21, get_12bits_start22, get_12bits_start23,
	get_12bits_start24, get_12bits_start25, get_12bits_start26, get_12bits_start27,
	get_12bits_start28, get_12bits_start29, get_12bits_start30, get_12bits_start31

	get_13bits_start00, get_13bits_start01, get_13bits_start02, get_13bits_start03,
	get_13bits_start04, get_13bits_start05, get_13bits_start06, get_13bits_start07,
	get_13bits_start08, get_13bits_start09, get_13bits_start10, get_13bits_start11,
	get_13bits_start12, get_13bits_start13, get_13bits_start14, get_13bits_start15,
	get_13bits_start16, get_13bits_start17, get_13bits_start18, get_13bits_start19,
	get_13bits_start20, get_13bits_start21, get_13bits_start22, get_13bits_start23,
	get_13bits_start24, get_13bits_start25, get_13bits_start26, get_13bits_start27,
	get_13bits_start28, get_13bits_start29, get_13bits_start30, get_13bits_start31

	get_14bits_start00, get_14bits_start01, get_14bits_start02, get_14bits_start03,
	get_14bits_start04, get_14bits_start05, get_14bits_start06, get_14bits_start07,
	get_14bits_start08, get_14bits_start09, get_14bits_start10, get_14bits_start11,
	get_14bits_start12, get_14bits_start13, get_14bits_start14, get_14bits_start15,
	get_14bits_start16, get_14bits_start17, get_14bits_start18, get_14bits_start19,
	get_14bits_start20, get_14bits_start21, get_14bits_start22, get_14bits_start23,
	get_14bits_start24, get_14bits_start25, get_14bits_start26, get_14bits_start27,
	get_14bits_start28, get_14bits_start29, get_14bits_start30, get_14bits_start31

	get_15bits_start00, get_15bits_start01, get_15bits_start02, get_15bits_start03,
	get_15bits_start04, get_15bits_start05, get_15bits_start06, get_15bits_start07,
	get_15bits_start08, get_15bits_start09, get_15bits_start10, get_15bits_start11,
	get_15bits_start12, get_15bits_start13, get_15bits_start14, get_15bits_start15,
	get_15bits_start16, get_15bits_start17, get_15bits_start18, get_15bits_start19,
	get_15bits_start20, get_15bits_start21, get_15bits_start22, get_15bits_start23,
	get_15bits_start24, get_15bits_start25, get_15bits_start26, get_15bits_start27,
	get_15bits_start28, get_15bits_start29, get_15bits_start30, get_15bits_start31

	get_16bits_start00, get_16bits_start01, get_16bits_start02, get_16bits_start03,
	get_16bits_start04, get_16bits_start05, get_16bits_start06, get_16bits_start07,
	get_16bits_start08, get_16bits_start09, get_16bits_start10, get_16bits_start11,
	get_16bits_start12, get_16bits_start13, get_16bits_start14, get_16bits_start15,
	get_16bits_start16, get_16bits_start17, get_16bits_start18, get_16bits_start19,
	get_16bits_start20, get_16bits_start21, get_16bits_start22, get_16bits_start23,
	get_16bits_start24, get_16bits_start25, get_16bits_start26, get_16bits_start27,
	get_16bits_start28, get_16bits_start29, get_16bits_start30, get_16bits_start31

	get_17bits_start00, get_17bits_start01, get_17bits_start02, get_17bits_start03,
	get_17bits_start04, get_17bits_start05, get_17bits_start06, get_17bits_start07,
	get_17bits_start08, get_17bits_start09, get_17bits_start10, get_17bits_start11,
	get_17bits_start12, get_17bits_start13, get_17bits_start14, get_17bits_start15,
	get_17bits_start16, get_17bits_start17, get_17bits_start18, get_17bits_start19,
	get_17bits_start20, get_17bits_start21, get_17bits_start22, get_17bits_start23,
	get_17bits_start24, get_17bits_start25, get_17bits_start26, get_17bits_start27,
	get_17bits_start28, get_17bits_start29, get_17bits_start30, get_17bits_start31

	get_18bits_start00, get_18bits_start01, get_18bits_start02, get_18bits_start03,
	get_18bits_start04, get_18bits_start05, get_18bits_start06, get_18bits_start07,
	get_18bits_start08, get_18bits_start09, get_18bits_start10, get_18bits_start11,
	get_18bits_start12, get_18bits_start13, get_18bits_start14, get_18bits_start15,
	get_18bits_start16, get_18bits_start17, get_18bits_start18, get_18bits_start19,
	get_18bits_start20, get_18bits_start21, get_18bits_start22, get_18bits_start23,
	get_18bits_start24, get_18bits_start25, get_18bits_start26, get_18bits_start27,
	get_18bits_start28, get_18bits_start29, get_18bits_start30, get_18bits_start31

	get_19bits_start00, get_19bits_start01, get_19bits_start02, get_19bits_start03,
	get_19bits_start04, get_19bits_start05, get_19bits_start06, get_19bits_start07,
	get_19bits_start08, get_19bits_start09, get_19bits_start10, get_19bits_start11,
	get_19bits_start12, get_19bits_start13, get_19bits_start14, get_19bits_start15,
	get_19bits_start16, get_19bits_start17, get_19bits_start18, get_19bits_start19,
	get_19bits_start20, get_19bits_start21, get_19bits_start22, get_19bits_start23,
	get_19bits_start24, get_19bits_start25, get_19bits_start26, get_19bits_start27,
	get_19bits_start28, get_19bits_start29, get_19bits_start30, get_19bits_start31

	get_20bits_start00, get_20bits_start01, get_20bits_start02, get_20bits_start03,
	get_20bits_start04, get_20bits_start05, get_20bits_start06, get_20bits_start07,
	get_20bits_start08, get_20bits_start09, get_20bits_start10, get_20bits_start11,
	get_20bits_start12, get_20bits_start13, get_20bits_start14, get_20bits_start15,
	get_20bits_start16, get_20bits_start17, get_20bits_start18, get_20bits_start19,
	get_20bits_start20, get_20bits_start21, get_20bits_start22, get_20bits_start23,
	get_20bits_start24, get_20bits_start25, get_20bits_start26, get_20bits_start27,
	get_20bits_start28, get_20bits_start29, get_20bits_start30, get_20bits_start31

	get_21bits_start00, get_21bits_start01, get_21bits_start02, get_21bits_start03,
	get_21bits_start04, get_21bits_start05, get_21bits_start06, get_21bits_start07,
	get_21bits_start08, get_21bits_start09, get_21bits_start10, get_21bits_start11,
	get_21bits_start12, get_21bits_start13, get_21bits_start14, get_21bits_start15,
	get_21bits_start16, get_21bits_start17, get_21bits_start18, get_21bits_start19,
	get_21bits_start20, get_21bits_start21, get_21bits_start22, get_21bits_start23,
	get_21bits_start24, get_21bits_start25, get_21bits_start26, get_21bits_start27,
	get_21bits_start28, get_21bits_start29, get_21bits_start30, get_21bits_start31

	get_22bits_start00, get_22bits_start01, get_22bits_start02, get_22bits_start03,
	get_22bits_start04, get_22bits_start05, get_22bits_start06, get_22bits_start07,
	get_22bits_start08, get_22bits_start09, get_22bits_start10, get_22bits_start11,
	get_22bits_start12, get_22bits_start13, get_22bits_start14, get_22bits_start15,
	get_22bits_start16, get_22bits_start17, get_22bits_start18, get_22bits_start19,
	get_22bits_start20, get_22bits_start21, get_22bits_start22, get_22bits_start23,
	get_22bits_start24, get_22bits_start25, get_22bits_start26, get_22bits_start27,
	get_22bits_start28, get_22bits_start29, get_22bits_start30, get_22bits_start31

	get_23bits_start00, get_23bits_start01, get_23bits_start02, get_23bits_start03,
	get_23bits_start04, get_23bits_start05, get_23bits_start06, get_23bits_start07,
	get_23bits_start08, get_23bits_start09, get_23bits_start10, get_23bits_start11,
	get_23bits_start12, get_23bits_start13, get_23bits_start14, get_23bits_start15,
	get_23bits_start16, get_23bits_start17, get_23bits_start18, get_23bits_start19,
	get_23bits_start20, get_23bits_start21, get_23bits_start22, get_23bits_start23,
	get_23bits_start24, get_23bits_start25, get_23bits_start26, get_23bits_start27,
	get_23bits_start28, get_23bits_start29, get_23bits_start30, get_23bits_start31

	get_24bits_start00, get_24bits_start01, get_24bits_start02, get_24bits_start03,
	get_24bits_start04, get_24bits_start05, get_24bits_start06, get_24bits_start07,
	get_24bits_start08, get_24bits_start09, get_24bits_start10, get_24bits_start11,
	get_24bits_start12, get_24bits_start13, get_24bits_start14, get_24bits_start15,
	get_24bits_start16, get_24bits_start17, get_24bits_start18, get_24bits_start19,
	get_24bits_start20, get_24bits_start21, get_24bits_start22, get_24bits_start23,
	get_24bits_start24, get_24bits_start25, get_24bits_start26, get_24bits_start27,
	get_24bits_start28, get_24bits_start29, get_24bits_start30, get_24bits_start31

	get_25bits_start00, get_25bits_start01, get_25bits_start02, get_25bits_start03,
	get_25bits_start04, get_25bits_start05, get_25bits_start06, get_25bits_start07,
	get_25bits_start08, get_25bits_start09, get_25bits_start10, get_25bits_start11,
	get_25bits_start12, get_25bits_start13, get_25bits_start14, get_25bits_start15,
	get_25bits_start16, get_25bits_start17, get_25bits_start18, get_25bits_start19,
	get_25bits_start20, get_25bits_start21, get_25bits_start22, get_25bits_start23,
	get_25bits_start24, get_25bits_start25, get_25bits_start26, get_25bits_start27,
	get_25bits_start28, get_25bits_start29, get_25bits_start30, get_25bits_start31

	get_26bits_start00, get_26bits_start01, get_26bits_start02, get_26bits_start03,
	get_26bits_start04, get_26bits_start05, get_26bits_start06, get_26bits_start07,
	get_26bits_start08, get_26bits_start09, get_26bits_start10, get_26bits_start11,
	get_26bits_start12, get_26bits_start13, get_26bits_start14, get_26bits_start15,
	get_26bits_start16, get_26bits_start17, get_26bits_start18, get_26bits_start19,
	get_26bits_start20, get_26bits_start21, get_26bits_start22, get_26bits_start23,
	get_26bits_start24, get_26bits_start25, get_26bits_start26, get_26bits_start27,
	get_26bits_start28, get_26bits_start29, get_26bits_start30, get_26bits_start31

	get_27bits_start00, get_27bits_start01, get_27bits_start02, get_27bits_start03,
	get_27bits_start04, get_27bits_start05, get_27bits_start06, get_27bits_start07,
	get_27bits_start08, get_27bits_start09, get_27bits_start10, get_27bits_start11,
	get_27bits_start12, get_27bits_start13, get_27bits_start14, get_27bits_start15,
	get_27bits_start16, get_27bits_start17, get_27bits_start18, get_27bits_start19,
	get_27bits_start20, get_27bits_start21, get_27bits_start22, get_27bits_start23,
	get_27bits_start24, get_27bits_start25, get_27bits_start26, get_27bits_start27,
	get_27bits_start28, get_27bits_start29, get_27bits_start30, get_27bits_start31

	get_28bits_start00, get_28bits_start01, get_28bits_start02, get_28bits_start03,
	get_28bits_start04, get_28bits_start05, get_28bits_start06, get_28bits_start07,
	get_28bits_start08, get_28bits_start09, get_28bits_start10, get_28bits_start11,
	get_28bits_start12, get_28bits_start13, get_28bits_start14, get_28bits_start15,
	get_28bits_start16, get_28bits_start17, get_28bits_start18, get_28bits_start19,
	get_28bits_start20, get_28bits_start21, get_28bits_start22, get_28bits_start23,
	get_28bits_start24, get_28bits_start25, get_28bits_start26, get_28bits_start27,
	get_28bits_start28, get_28bits_start29, get_28bits_start30, get_28bits_start31

	get_29bits_start00, get_29bits_start01, get_29bits_start02, get_29bits_start03,
	get_29bits_start04, get_29bits_start05, get_29bits_start06, get_29bits_start07,
	get_29bits_start08, get_29bits_start09, get_29bits_start10, get_29bits_start11,
	get_29bits_start12, get_29bits_start13, get_29bits_start14, get_29bits_start15,
	get_29bits_start16, get_29bits_start17, get_29bits_start18, get_29bits_start19,
	get_29bits_start20, get_29bits_start21, get_29bits_start22, get_29bits_start23,
	get_29bits_start24, get_29bits_start25, get_29bits_start26, get_29bits_start27,
	get_29bits_start28, get_29bits_start29, get_29bits_start30, get_29bits_start31

	get_30bits_start00, get_30bits_start01, get_30bits_start02, get_30bits_start03,
	get_30bits_start04, get_30bits_start05, get_30bits_start06, get_30bits_start07,
	get_30bits_start08, get_30bits_start09, get_30bits_start10, get_30bits_start11,
	get_30bits_start12, get_30bits_start13, get_30bits_start14, get_30bits_start15,
	get_30bits_start16, get_30bits_start17, get_30bits_start18, get_30bits_start19,
	get_30bits_start20, get_30bits_start21, get_30bits_start22, get_30bits_start23,
	get_30bits_start24, get_30bits_start25, get_30bits_start26, get_30bits_start27,
	get_30bits_start28, get_30bits_start29, get_30bits_start30, get_30bits_start31

	get_31bits_start00, get_31bits_start01, get_31bits_start02, get_31bits_start03,
	get_31bits_start04, get_31bits_start05, get_31bits_start06, get_31bits_start07,
	get_31bits_start08, get_31bits_start09, get_31bits_start10, get_31bits_start11,
	get_31bits_start12, get_31bits_start13, get_31bits_start14, get_31bits_start15,
	get_31bits_start16, get_31bits_start17, get_31bits_start18, get_31bits_start19,
	get_31bits_start20, get_31bits_start21, get_31bits_start22, get_31bits_start23,
	get_31bits_start24, get_31bits_start25, get_31bits_start26, get_31bits_start27,
	get_31bits_start28, get_31bits_start29, get_31bits_start30, get_31bits_start31

	get_32bits_start00, get_32bits_start01, get_32bits_start02, get_32bits_start03,
	get_32bits_start04, get_32bits_start05, get_32bits_start06, get_32bits_start07,
	get_32bits_start08, get_32bits_start09, get_32bits_start10, get_32bits_start11,
	get_32bits_start12, get_32bits_start13, get_32bits_start14, get_32bits_start15,
	get_32bits_start16, get_32bits_start17, get_32bits_start18, get_32bits_start19,
	get_32bits_start20, get_32bits_start21, get_32bits_start22, get_32bits_start23,
	get_32bits_start24, get_32bits_start25, get_32bits_start26, get_32bits_start27,
	get_32bits_start28, get_32bits_start29, get_32bits_start30, get_32bits_start31
*/
};

class tzp_coding {
	public:
		tzp_coding();
		tzp_coding(uint32_t*);

		void bit_write(uint32_t, uint32_t);
		void bit_flush();
		int32_t Compression(uint32_t *, uint32_t *, int32_t, uint32_t*);
		void Decompression(uint32_t *, uint32_t *, int32_t, uint32_t, int32_t);

		int get_type();
		void set_size(int size);
		void pad_vec(unsigned char*);


	private:
		uint64_t buffer;
		uint32_t Fill;
		uint32_t *data;
		uint64_t written;



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

tzp_coding::tzp_coding(uint32_t *out) {

}

tzp_coding::tzp_coding() {

	this->Fill = 0;
	this->buffer = 0;
	this->written = 0;

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






void tzp_coding::bit_flush() {
	if (Fill > 32) {
		buffer = buffer << (64 - Fill);
		*(data++) = buffer >> 32;
		*(data++) = buffer & ((1UL<<32)-1);
		written += 2;
		Fill = 0;
	}

	if (Fill > 0 ) {
		*(data++) = buffer << (32 - Fill) & ((1UL << 32) - 1);
		written++;
	}

	buffer = 0;
	Fill = 0;
}

void tzp_coding::bit_write(uint32_t value, uint32_t bits) {
	uint32_t Mask;

	if (bits == 0) {
		return;
    }

	Mask = (1 << bits) - 1;

	if (bits == 32) {
		buffer = (buffer << bits) | value;
	} else {
		buffer = (buffer << bits) | (value & Mask);
	}

	Fill += bits;
	if (Fill >= 32) {
		*(data++) = (buffer >> (Fill - 32)) & ((1UL << 32) - 1);
		written++;
		Fill -= 32;
	}
}


int32_t tzp_coding::Compression(uint32_t* input, uint32_t* output, int32_t size, uint32_t *nbits) {

	int32_t i = 0, ret = 0;
	*nbits = find_bits(input, size);
//	unsigned char *tmp = (unsigned char*)output;

//	this->bit = 0;
//	this->pos = 0;
//	this->cur = 0;

	this->data = output;

	for (i = 0; i < size; i++) {
		bit_write(input[i], *nbits);
	}

//	this->len += 32 * (ret / 32) + 32;
//	tmp[this->pos] = this->cur;

	return (ret / 32) + 1;
}






/// /////////////////////////////////////////////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////////////////////////


void tzp_coding::Decompression(uint32_t* input, uint32_t* output, int32_t size,
	uint32_t start, int32_t num_bits) {

	int32_t s = start, i = 0;

	for (i = 0; i < size; i++) {

 		TZP_unpacker[32 * num_bits + s](input, output);
		output++;

//		printf("Decompressed Number %d is %d\n", i, output[i]);
		s += num_bits;

		if (s >= 32) {
			s -= 32;
			input++;
		}
	}
}

/*
// Starting from bit b, decode size integers by reading nb bits for each
int32_t tzp_coding::Decompression(uint32_t* input, uint32_t* output, int32_t size,
	uint32_t start, int32_t num_bits) {

		unsigned char *tmp_in = (unsigned char *)input;
		int32_t _p = start / 8;
		int32_t _b = start - _p * 8, shift = 0, mask = 0, i = 0, s = size, num = num_bits;
		unsigned char _c = tmp_in[_p], temp;
		uint32_t val = 0;
		this->len = 10000000000;

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
*/

