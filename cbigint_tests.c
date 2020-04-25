
#define CBIGINT_IMPLEMENTATION
#include "cbigint.h"


#include <CUnit/Automated.h>
#include <stdio.h>


/* add tests */
void add_test()
{
	cbigint a;
	cbi_init(&a);


	CU_ASSERT_EQUAL(CVEC_long_SZ, a.mag.capacity);
	CU_ASSERT_EQUAL(1, a.mag.size);

	CU_ASSERT_EQUAL(0, a.mag.a[0]);

	cbigint b = { 0 };

#define TEST_NUM 900000009

	cbi_set(&b, TEST_NUM);

	cbigint c = { 0 };
	cbi_add(&c, &a, &b);

	CU_ASSERT_EQUAL(TEST_NUM, c.mag.a[0]);

	char buf[100];
	for (int i=0; i<1000; i++) {
		cbi_add(&c, &c, &b);
		// TODO test every step?
	}

	cbi_tocstr(&c, buf);
	CU_ASSERT_STRING_EQUAL("900900009009", buf);


	cbi_free(&a);
	cbi_free(&b);
	cbi_free(&c);
}


void sub_test()
{

}














