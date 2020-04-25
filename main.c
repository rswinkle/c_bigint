
#include "cbigint.h"

#include <stdio.h>


int main(int argc, char** argv)
{
	char s_a[] = "999";
	char s_b[] = "-109";
	char s_3[] = "3";
	char s_100[] = "100";
	char s_10[] = "10";

	cbigint a = { 0 }, b = { 0 };
	cbigint three, hundred, ten;

	cbi_fromcstr(&a, s_a);
	cbi_fromcstr(&b, s_b);

	cbi_initfromcstr(&three, s_3);
	cbi_initfromcstr(&hundred, s_100);
	cbi_initfromcstr(&ten, s_10);

	cbigint p = { 0 };
	char buf[1024];

	cbi_add(&p, &b, &a);
	printf("%s + %s = %s\n", s_a, s_b, cbi_tocstr(&p, buf));

	cbi_sub(&p, &b, &a);
	printf("%s - %s = %s\n", s_b, s_a, cbi_tocstr(&p, buf));
	cbi_mult(&p, &three, &a);
	printf("%s * %s = %s\n", s_3, s_a, cbi_tocstr(&p, buf));
	cbi_mult(&p, &a, &three);
	printf("%s * %s = %s\n", s_a, s_3, cbi_tocstr(&p, buf));
	cbi_mult(&p, &hundred, &ten);
	printf("%s * %s = %s\n", s_100, s_10, cbi_tocstr(&p, buf));


	cbi_free(&a);
	cbi_free(&b);
	cbi_free(&p);
	cbi_free(&three);
	cbi_free(&hundred);
	cbi_free(&ten);

	return 0;
}
