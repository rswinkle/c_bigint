
#define CVECTOR_long_IMPLEMENTATION
#include "cbigint.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

// cbi_init0() ?
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

// can/should I use the comma operator to "return" n?
#define cbi_abs(n) (((n)->sign = (n)->sign ? 1 : 0), n)

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
	// TODO error if a > CBI_BASE, have to break up
	n->mag.size = 0;
	n->sign = 0;
	if (!a)
		return cvec_push_long(&n->mag, 0);

	n->sign = 1;
	if (a < 0) {
		a = -a;
		n->sign = -1;
	}
	if (a > CBI_BASE) {
		if (!cvec_push_long(&n->mag, a/CBI_BASE))
			return 0;
		a %= CBI_BASE;
	}
	return cvec_push_long(&n->mag, a);
}

// both of these are unecessary but for completionist/convenience sake
int cbi_reserve(cbigint* n, size_t size)
{
	return cvec_reserve_long(&n->mag, size);
}
void cbi_negate(cbigint* n)
{
	n->sign = -n->sign;
}

// on immediate error, do I clear/zero out n?
cbigint* cbi_read(cbigint* n, FILE* input)
{
	char* string = NULL, *tmp_str = NULL;
	int temp;
	int i = 0;
	int max_len = 4096;

	if (feof(input)) {
		return NULL;
	}

	while (isspace(temp = fgetc(input)));

	if (temp != '-' && temp != '+' && !isdigit(temp)) {
		return NULL;
	}

	if (!(string = (char*)malloc(max_len+1)))
		return NULL;

	string[i++] = temp;

	while (1) {
		temp = fgetc(input);

		if (temp == EOF || !isdigit(temp)) {
			if (!i) {
				free(string);
				return NULL;
			}
			tmp_str = (char*)realloc(string, i+1);
			if (!tmp_str) {
				free(string);
				return NULL;
			}
			string = tmp_str;

			// no-op if temp is EOF?
			ungetc(temp, input);
			break;
		}

		if (i == max_len) {
			tmp_str = (char*)realloc(string, max_len*2+1);
			if (!tmp_str) {
				free(string);
				return NULL;
			}
			string = tmp_str;
			max_len *= 2;
		}

		string[i] = temp;
		i++;
	}
	string[i] = '\0';

	// unfortunately this re-checks for valid string
	n = cbi_fromcstr(n, string);
	free(string);

	return n;
}

cbigint* cbi_initfromcstr(cbigint* n, const char* s)
{
	cvec_long(&n->mag, 0, 0);
	return cbi_fromcstr(n, s);
}

cbigint* cbi_fromcstr(cbigint* n, const char* s)
{
	if (!s) {
		return NULL;
	}

	// TODO correctly handle sign
	int sign = 1;
	char* p = (char*)s;
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
		return n;
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
			return NULL;
		}
		p[start] = 0; // cut off segment that was just converted
		start -= CBI_POWER;
	}
	// get everything that was left if necessary
	if (start != -CBI_POWER) {
		if (!cvec_insert_long(&n->mag, 0, atol(&p[0]))) {
			free(buf);
			return NULL;
		}
	}

	free(buf);
	return n;

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

	cvec_reserve_long(&s->mag, a.mag.size > b.mag.size ? a.mag.size : b.mag.size + 1);

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

cbigint* cbi_sub(cbigint* d, cbigint* a_in, cbigint* b_in)
{
	cbigint a, b;
	cbi_copy(&a, a_in);
	cbi_copy(&b, b_in);

	d->mag.size = 0;

	// negative - positive = negate(positive + positive)
	if (a.sign == -1 && b.sign == 1) {
		a.sign = 1;
		cbi_add(d, &a, &b);
		d->sign = -1;
		cvec_free_long(&a.mag);
		cvec_free_long(&b.mag);
		return d;
	}

	// positive - negative = positive + positive
	if (a.sign == 1 && b.sign == -1) {
		b.sign = 1;
		cbi_add(d, &a, &b);
		cvec_free_long(&a.mag);
		cvec_free_long(&b.mag);
		return d;
	}


	cbigint t;
	int final_sign = 0;

	// both pos or neg
	int cmp = cbi_compare_mag(&a, &b);
	if (cmp < 0) {
		final_sign = (b.sign == 1) ? -1 : 1;
		t = a;
		a = b;
		b = t;
	} else if (!cmp) {
		cbi_zero(d);
		cvec_free_long(&a.mag);
		cvec_free_long(&b.mag);
		return d;
	} else {
		final_sign = a.sign;
	}

	// thanks to swap, we know a is larger and is the max length result can be
	cvec_reserve_long(&d->mag, a.mag.size);

	long a_digit, b_digit;
	long a_i = a.mag.size-1, b_i = b.mag.size-1;
	int j;
	for (; a_i >= 0 && b_i >= 0; --a_i, --b_i) {
		a_digit = a.mag.a[a_i];
		b_digit = b.mag.a[b_i];

		if (a_digit < b_digit) {
			j = a_i;
			while (j > 0) {
				j--;
				if (a.mag.a[j] != 0) {
					a.mag.a[j]--;
					j++;
					while (j != a_i) {
						a.mag.a[j++] += CBI_BASE-1;
					}
					// TODO not strictly necessary
					a.mag.a[a_i] += CBI_BASE;

					a_digit += CBI_BASE;
					break;
				}
			}
		}

		cvec_insert_long(&d->mag, 0, a_digit - b_digit);
	}

	// add rest of a
	for ( ; a_i >= 0; --a_i) {
		cvec_insert_long(&d->mag, 0, a.mag.a[a_i]);
	}

	// remove leading 0's
	int i = 0;
	while (i < d->mag.size-1 && !d->mag.a[i])
		i++;
	if (i)
		cvec_erase_long(&d->mag, 0, i-1);

	// TODO for now size 0 and size 1 with number == 0 are both
	// representations of 0
	d->sign = (d->mag.size == 1 && !d->mag.a[0]) ? 0 : final_sign;

	cvec_free_long(&a.mag);
	cvec_free_long(&b.mag);

	return d;
}

cbigint* cbi_mult(cbigint* p, cbigint* a_in, cbigint* b_in)
{
	cbigint a, b;

	if (!a_in->sign || !b_in->sign) {
		cbi_zero(p);
		return p;
	}

	cbi_copy(&a, a_in);
	cbi_copy(&b, b_in);

	// TODO sooo, all the operations require p to be initialized I guess
	cbi_zero(p); // ?
	
	cbigint t;
	// algorithm doesn't work if a ("top number") is shorter than b
	// swapping is easier than changing the alg to work either way
	if (a.mag.size < b.mag.size) {
		t = a;
		a = b;
		b = t;
	}



	// Make sure both p and r are large enough to hold potential values ahead of time
	// so no need to realloc in middle of calculation
	cbigint r;
	cvec_long(&r.mag, 0, a.mag.size+b.mag.size);
	r.sign = 1;   // not sure if necessary

	cvec_reserve_long(&p->mag, a.mag.size+b.mag.size);

	long temp, carry, a_digit, b_digit;
	int i, j, shift = 0;
	for (i=b.mag.size-1; i>=0; --i, ++shift) {
		r.mag.size = 0;
		b_digit = b.mag.a[i];
		carry = 0;

		// perform the shift
		// since r is empty, push works as well as insert(0)
		for (int k=0; k<shift; ++k) {
			cvec_push_long(&r.mag, 0);
		}

		for (j=a.mag.size-1; j>=0; --j) {
			a_digit = a.mag.a[j];

			// This line is the limiting factor for CBI_BASE
			// CBI_BASE^2 < LONG_MAX must hold.  If sizeof(long) == 8
			// 10^9 is the largest power of 10 that remains true.
			// It'd be 10^4 for a 32-bit long
			temp = a_digit * b_digit + carry;
			if (temp >= CBI_BASE) {
				carry = temp / CBI_BASE;
				temp %= CBI_BASE;
			} else {
				carry = 0;
			}

			cvec_insert_long(&r.mag, 0, temp);
		}

		if (carry) {
			cvec_insert_long(&r.mag, 0, carry);
		}

		cbi_add(p, p, &r);
	}

	p->sign = 1;
	if (a.sign != b.sign)
		p->sign = -1;

	cvec_free_long(&r.mag);
	cvec_free_long(&a.mag);
	cvec_free_long(&b.mag);

	return p;
}

cbigint* cbi_div(cbigint* q, cbigint* a_in, cbigint* b_in)
{
	cbigint a, b;

	// division by 0
	if (!b_in->sign) {
		return NULL;
	}

	int cmp = cbi_compare_mag(a_in, b_in);
	if (!a_in->sign || cmp < 0) {
		cbi_zero(q);
		return q;
	}

	q->mag.size = 0;
	q->sign = 1;
	if (a_in->sign != b_in->sign) {
		q->sign = -1;
	}

	// divide by 1
	if (b_in->mag.size == 1 && b_in->mag.a[0] == 1) {
		cvec_insert_array_long(&q->mag, 0, a_in->mag.a, a_in->mag.size);
		return q;
	}

	if (!cmp) {
		cvec_push_long(&q->mag, 1);
		return q;
	}

	cbi_copy(&a, a_in);
	cbi_copy(&b, b_in);

	cvec_reserve_long(&q->mag, a.mag.size);

	//TODO There's got to be a better/more efficient way
	//than traditional/old school long division.  I feel like
	//there is some simpler-to-program equivalent on the tip
	//of my brain
	
	int i = 0, j = 0;
	long cut;
	cbigint dividend_part, tmp;
	dividend_part.sign = tmp.sign = 1;

	cvec_long(&dividend_part.mag, 0, a.mag.size);
	cvec_long(&tmp.mag, 0, a.mag.size);

	cvec_insert_array_long(&dividend_part.mag, 0, a.mag.a, b.mag.size);
	i += b.mag.size;

	cmp = cbi_compare_mag(&dividend_part, &b);

	do {

		// TODO create cbi_copy()/cvec_copy that doesn't assume cap == 0
		tmp.mag.size = 0;
		cvec_insert_array_long(&tmp.mag, 0, b.mag.a, b.mag.size);

		if (cmp < 0) {
			cvec_push_long(&dividend_part.mag, a.mag.a[i++]);
			// subtract div_part[0]*b from dividend_part

			cut = dividend_part.mag.a[0];
			

			cbi_multl(&tmp, cut);
			cbi_sub(&dividend_part, &dividend_part, &tmp);
		} else {
			// subtract (div_part[0]/(b[0]+1))*b from dividend_part
			cut = dividend_part.mag.a[0] / (b.mag.a[0]+1);
			cbi_multl(&tmp, cut);
			cbi_sub(&dividend_part, &dividend_part, &tmp);
		}


		while (cbi_compare_mag(&dividend_part, &b) >= 0) {
			cbi_sub(&dividend_part, &dividend_part, &b);
			cut++;
		}

		cvec_push_long(&q->mag, cut);

		// could optimize with insert_array_long
		while (dividend_part.mag.size < b.mag.size && i < a.mag.size) {
			cvec_push_long(&dividend_part.mag, a.mag.a[i++]);
		}
		cmp = cbi_compare_mag(&dividend_part, &b);
	} while (i < a.mag.size || cmp >= 0);


	// dividend_part should contain the remainder, if I added that
	// as an output parameter
	cvec_free_long(&a.mag);
	cvec_free_long(&b.mag);
	cvec_free_long(&dividend_part.mag);
	cvec_free_long(&tmp.mag);

	return q;
}

// TODO hard to imagine dealing with numbers big enough...
// maybe I should have cbi_pow(cbi* e, long, cbi* x)?
cbigint* cbi_pow(cbigint* e, cbigint* a, cbigint* x)
{
	return NULL;
}

// TODO testing
cbigint* cbi_powl(cbigint* e, cbigint* a, unsigned long x)
{
	if (!x) {
		e->mag.size = 0;
		e->sign = 1;
		cvec_push_long(&e->mag, 1);
		return e;
	}

	e->mag.size = 0;
	e->sign = 1;
	cvec_insert_array_long(&e->mag, 0, a->mag.a, a->mag.size);
	if (x == 1) {
		return e;
	}
	
	int parity = x & 0x1;
	while (x > 1) {
		cbi_mult(e, e, e);
		x >>= 1;
	}
	if (parity)
		cbi_mult(e, e, a);

	return e;
}




// modifies first parameter and returns it, equivalent to set + op
// TODO optimization path for these funcs if x is < CBI_BASE?
cbigint* cbi_addl(cbigint* s, long x)
{
	cbigint a = { 0 };
	cbi_set(&a, x);
	cbi_add(s, s, &a);
	cvec_free_long(&a.mag);
	return s;
}
cbigint* cbi_subl(cbigint* d, long x)
{
	cbigint a = { 0 };
	cbi_set(&a, x);
	cbi_sub(d, d, &a);
	cvec_free_long(&a.mag);
	return d;
}
cbigint* cbi_multl(cbigint* p, long x)
{
	cbigint a = { 0 };
	cbi_set(&a, x);
	cbi_mult(p, p, &a);
	cvec_free_long(&a.mag);
	return p;
}
cbigint* cbi_divl(cbigint* d, long x)
{
	cbigint a = { 0 };
	cbi_set(&a, x);
	cbi_div(d, d, &a);
	cvec_free_long(&a.mag);
	return d;
}






