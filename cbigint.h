
#ifndef CBIGINT_H
#define CBIGINT_H

#include "cvector_long.h"

#include <stdio.h>

/* currently only support 64 bit */
#define CBI_POWER 9
#define CBI_BASE 1000000000

#define cbi_maxstrlen(n) ((n)->mag.size*CBI_POWER+2)

typedef struct cbigint
{
	cvector_long mag;
	int sign;
} cbigint;

int cbi_init(cbigint* n);
void cbi_free(void* n);

// return dst?
int cbi_copy(cbigint* dest, cbigint* src);

int cbi_compare(cbigint* a, cbigint* b);
int cbi_compare_mag(cbigint* a, cbigint* b);

// hmm
int cbi_zero(cbigint* n);
int cbi_set(cbigint* n, long a);
int cbi_reserve(cbigint* n, size_t size);

void cbi_negate(cbigint* n);

cbigint* cbi_read(cbigint* n, FILE* input);
cbigint* cbi_initfromcstr(cbigint* n, const char* s);
cbigint* cbi_fromcstr(cbigint* n, const char* s);

size_t cbi_strbuf_sz(cbigint* n);
char* cbi_tocstr(cbigint* n, char* out);


cbigint* cbi_add(cbigint* s, cbigint* a, cbigint* b);
cbigint* cbi_sub(cbigint* s, cbigint* a, cbigint* b);
cbigint* cbi_mult(cbigint* s, cbigint* a, cbigint* b);
cbigint* cbi_div(cbigint* s, cbigint* a, cbigint* b);

cbigint* cbi_addl(cbigint* s, long x);
cbigint* cbi_subl(cbigint* d, long x);
cbigint* cbi_multl(cbigint* s, long x);
cbigint* cbi_addl(cbigint* s, long x);








#endif
