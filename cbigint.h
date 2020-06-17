
#ifndef CBIGINT_H
#define CBIGINT_H

#include "cvector_long.h"

#include <stdio.h>

// multiplication is the limiting factor for CBI_BASE
// CBI_BASE^2 < LONG_MAX must hold.  If sizeof(long) == 8
// 10^9 is the largest power of 10 that remains true.
// It'd be 10^4 for a 32-bit long
//
// lower powers, 1 and 2 are good for testing/rooting out bugs
#define CBI_POWER 1
#define CBI_BASE 10
//#define CBI_POWER 9
//#define CBI_BASE 1000000000

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
int cbi_setl(cbigint* n, long a);
int cbi_setul(cbigint* n, unsigned long a);
int cbi_setll(cbigint* n, long long a);
int cbi_set(cbigint* n, cbigint* a);
long cbi_tol(cbigint* n);
unsigned long cbi_toul(cbigint* n);
long long cbi_toll(cbigint* n);
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
cbigint* cbi_multl(cbigint* p, long x);
cbigint* cbi_divl(cbigint* q, long x);

cbigint* cbi_addll(cbigint* s, long a, long b);
cbigint* cbi_subll(cbigint* d, long a, long b);
cbigint* cbi_multll(cbigint* p, long a, long b);
cbigint* cbi_divll(cbigint* q, long a, long b);

// name?
//cbigint* cbi_addl(cbigint* s, cbigint* a, long b);
//cbigint* cbi_subl(cbigint* d, cbigint* a, long b);
//cbigint* cbi_multl(cbigint* p, cbigint* a, long b);
//cbigint* cbi_divl(cbigint* q, cbigint* a, long b);


cbigint* cbi_pow(cbigint* e, cbigint* a, cbigint* x);
cbigint* cbi_powl(cbigint* e, cbigint* a, unsigned long x);






#endif
