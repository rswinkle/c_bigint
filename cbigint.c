
#define CVECTOR_long_IMPLEMENTATION
#include "cbigint.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

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
	// works because copy sets cap to 0 before attempting allocation
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
int cbi_zero(cbigint* n)
{
	n->mag.size = 0;
	n->sign = 0;
	return cvec_push_long(&n->mag, 0);
}

// TODO change to using uint64_t?  or unsigned long?
int cbi_set(cbigint* n, long a)
{
	n->mag.size = 0;
	if (a < 0) {
		n->sign = -1;
		return cvec_push_long(&n->mag, -a);
	}
	n->sign = a > 0;
	return cvec_push_long(&n->mag, a);
}

// unecessary but for completionist sake
void cbi_negate(cbigint* n)
{
	n->sign = -n->sign;
}

// return dest?
int cbi_fromcstr(cbigint* n, const char* s)
{
	if (!s) {
		return NULL;
	}

	// TODO correctly handle sign
	int sign = 1;
	char* p = s;
	if (s[0] == '-') {
		sign = -1;
		p++;
	} else if (s[0] == '+') {
		p++;
	}

	// TODO calculate minimum buf size
	// A whole megabyte for heaven's sake
	char* buf = malloc(1 << 20);

	/// make sure everything else is a digit
	int len = 0;
	for (char* c = p; *c; len++, c++) {
		if (!isdigit(*c)) {
			free(buf);
			return NULL;
		}
		buf[len] = *c;
	}
	buf[len] = 0;
	p = buf;

	// eat up leading 0's first and if no non-zero
	// just set n to 0;
	while (*p == '0') {
		p++;
		len--;
	}

	if (!len) {
		free(buf);
		cbi_zero(n);
		return 1;;
	}


	//int num_digits = len / CBI_POWER;
	
	/* if POwER were 2
	"1234567"
	01 23 45 67
	*/
	
	n->mag.size = 0;
	n->sign = sign;

	// algorithm converts CBI_POWER digits at a time from the back
	// prepending to the list
	int start = len-CBI_POWER;

	// don't have to use strtol because we already verified
	// only digits
	while (start > 0) {
		if (!cvec_insert_long(&n->mag, 0, atol(&p[start]))) {
			free(buf);
			return 0;
		}
		p[start] = 0; // cut off segment that was just converted
		start -= CBI_POWER;
	}
	// get everything that was left if necessary
	if (start != -CBI_POWER) {
		if (!cvec_insert_long(&n->mag, 0, atol(&p[0]))) {
			free(buf);
			return 0;
		}
	}

	free(buf);
	return 1;

}

// include +1 for \0 or not?
size_t cbi_strbuf_sz(cbigint* n)
{
	size_t sz = 0;
	if (n->sign == -1)
		sz++;
	
	for (int i=0; i<n->mag.size; ++i) {
		// Know it's long, change if necessary
		// don't pad with 0's on highest digit
		if (i)
			sz += snprintf(NULL, 0, "%0*ld", CBI_POWER, n->mag.a[i]);
		else
			sz += snprintf(NULL, 0, "%ld", n->mag.a[i]);
	}
	return sz+1;
}

// assumes out is not NULL and is at least cbi_strbuf_sz(n) bytes
char* cbi_tocstr(cbigint* n, char* out)
{
	char* p = out;
	if (n->sign == -1)
		*p++ = '-';
	
	for (int i=0; i<n->mag.size; ++i) {
		if (i)
			p += sprintf(p, "%0*ld", CBI_POWER, n->mag.a[i]);
		else
			p += sprintf(p, "%ld", n->mag.a[i]);
	}

	return out;
}


cbigint* cbi_add(cbigint* s, cbigint* a_in, cbigint* b_in)
{
	cbigint a, b;
	cbi_copy(&a, a_in);
	cbi_copy(&b, b_in);

	s->mag.size = 0;

	// TODO future idea, reverse both a and b first so can
	// add left to right?  and/or build s in reverse with push
	// and reverse at the end
	
	if (a.sign == b.sign) {
		// standard addition looping from least significant
		// adding in carries
		long temp, carry = 0;
		long a_i = a.mag.size-1, b_i = b.mag.size-1;
		for (; a_i >= 0 && b_i >= 0; --a_i, --b_i) {
			temp = a.mag.a[a_i] + b.mag.a[b_i] + carry;
			if (temp >= CBI_BASE) {
				carry = 1;
				temp -= CBI_BASE;
			} else {
				carry = 0;
			}
			cvec_insert_long(&s->mag, 0, temp);
		}

		if (a_i < 0 && b_i < 0 && carry) {
			cvec_insert_long(&s->mag, 0, carry);
		}

		for (; a_i >= 0; --a_i) {
			temp = a.mag.a[a_i] + carry;
			if (temp >= CBI_BASE) {
				carry = 1;
				temp -= CBI_BASE;
			} else {
				carry = 0;
			}
			cvec_insert_long(&s->mag, 0, temp);
		}
		for (; b_i >= 0; --b_i) {
			temp = b.mag.a[b_i] + carry;
			if (temp >= CBI_BASE) {
				carry = 1;
				temp -= CBI_BASE;
			} else {
				carry = 0;
			}
			cvec_insert_long(&s->mag, 0, temp);
		}

		s->sign = a.sign;

	} else {
		if (!a.sign) {
			s->mag.size = 0;
			cvec_insert_array_long(&s->mag, 0, b.mag.a, b.mag.size);
			s->sign = b.sign;
		} else if (!b.sign) {
			s->mag.size = 0;
			cvec_insert_array_long(&s->mag, 0, a.mag.a, a.mag.size);
			s->sign = a.sign;
		} else if (a.sign == -1) {
			a.sign = 1;
			cbi_sub(s, &b, &a);
		} else {
			b.sign = 1;
			cbi_sub(s, &a, &b);
		}
	}

	// could call cbi_free but meh
	cvec_free_long(&a.mag);
	cvec_free_long(&b.mag);

	return s;
}


cbigint* cbi_sub(cbigint* s, cbigint* a, cbigint* b)
{
}

cbigint* cbi_mult(cbigint* s, cbigint* a, cbigint* b)
{
}

cbigint* cbi_div(cbigint* s, cbigint* a, cbigint* b)
{
}









