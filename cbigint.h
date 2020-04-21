
#ifndef CBIGINT_H
#define CBIGINT_H

#include "cvector_long.h"

typedef struct cbigint
{
	cvector_long mag;
	int sign;
} cbigint;

int cbi_init(cbigint* n);
void cbi_free(void* n);

// return dst?
int cbi_copy(cbigint* dst, cbigint* src);

int cbi_compare(cbigint* a, cbigint* b);
int cbi_compare_mag(cbigint* a, cbigint* b);

// hmm
void cbi_zero(cbigint* n);
void cbi_set(cbigint* n, long a);

void cbi_negate(cbigint* n);

// return dst?
int cbi_fromcstr(cbigint* n, const char* s);

// if out is NULL, fill len with size buffer has to be including '\0'
void cbi_tocstr(cbigint* n, char* out, int* len);


cbigint* cbi_add(cbigint* s, cbigint* a, cbigint* b);
cbigint* cbi_sub(cbigint* s, cbigint* a, cbigint* b);
cbigint* cbi_mult(cbigint* s, cbigint* a, cbigint* b);
cbigint* cbi_div(cbigint* s, cbigint* a, cbigint* b);









#endif
