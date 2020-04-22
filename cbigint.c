
#define CVECTOR_long_IMPLEMENTATION
#include "cbigint.h"

/* currently only support 64 bit */
#define POWER 9
#define BASE 1000000000


int cbi_init(cbigint* n)
{
	if (!cvec_long(&n->mag, 0, 0)) {
		return 0;
	}
	cvec_push_long(&n->mag, 0);
	n->sign = 0;
	return 1;
}

void cbi_free(void* n)
{
	cbigint* tmp = (cbigint*)n;
	cvec_free_long(&tmp->mag);
	tmp->sign = 0;
}

// return dst?
// 
// TODO change cvector library so copy functions
// return int, better documentation comment about how
// it assumes dest and source are valid pointers to vectors
// but that dest has capacity 0.  Or for the latter part,
// change copy so that it handles the > 0 capacity case
// 
// Also why cvec_long_copy not cvec_copy_long?  Every other cvec
// function ends with the _type.
int cbi_copy(cbigint* dest, cbigint* src)
{
	cvec_long_copy(&dest->mag, &src->mag);
	dest->sign = src->sign;

	// hackish way to test for success of copy function
	return dest->mag.capacity == src->mag.capacity;
}

int cbi_compare(cbigint* a, cbigint* b)
{
	if (a->sign < b->sign)
		return -1;
	if (a->sign > b->sign)
		return 1;
	if (a->sign == 0)
		return 0;

	if (a->sign == 1) {
		return cbi_compare_mag(a, b);
	}

	return cbi_compare_mag(b, a);
}

int cbi_compare_mag(cbigint* a, cbigint* b)
{
	if (a->mag.size < b->mag.size)
		return -1;
	if (a->mag.size > b->mag.size)
		return 1;

	for (int i=0; i<a->mag.size; ++i) {
		if (a->mag.a[i] < b->mag.a[i])
			return -1;
		if (a->mag.a[i] > b->mag.a[i])
			return 1;
	}

	return 0;
}


// hmm return int, could fail
void cbi_zero(cbigint* n)
{
	n->mag.size = 0;
	cvec_push_long(&n->mag, 0);
	n->sign = 0;
}

// TODO change to using uint64_t?  or unsigned long?
void cbi_set(cbigint* n, long a)
{
	n->mag.size = 0;
	if (a < 0) {
		cvec_push_long(&n->mag, -a);
		n->sign = -1;
		return;
	}
	cvec_push_long(&n->mag, a);
	n->sign = a > 0;
}

// unecessary but for completionist sake
void cbi_negate(cbigint* n)
{
	n->sign = -n->sign;
}

// return dest?
int cbi_fromcstr(cbigint* n, const char* s);

// if out is NULL, fill len with size buffer has to be including '\0'
void cbi_tocstr(cbigint* n, char* out, int* len);


cbigint* cbi_add(cbigint* s, cbigint* a, cbigint* b);
cbigint* cbi_sub(cbigint* s, cbigint* a, cbigint* b);
cbigint* cbi_mult(cbigint* s, cbigint* a, cbigint* b);
cbigint* cbi_div(cbigint* s, cbigint* a, cbigint* b);









