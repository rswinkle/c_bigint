
#define CBIGINT_IMPLEMENTATION
#include "cbigint.h"


#include <CUnit/Automated.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>

// copied from my c_utils
char* freadline(FILE* input);
char* freadstring(FILE* input, int delim, size_t max_len);

#define xstr(s) str(s)
#define str(s) #s

void add_test()
{
	char buf[100];
	cbigint a;
	cbi_init(&a);

	CU_ASSERT_EQUAL(CVEC_long_SZ, a.mag.capacity);
	CU_ASSERT_EQUAL(1, a.mag.size);

	CU_ASSERT_EQUAL(0, a.mag.a[0]);

	cbigint b = { 0 };

#define TEST_NUM 900000009

	cbi_setl(&b, TEST_NUM);

	cbigint c = { 0 };
	cbi_add(&c, &a, &b);

	//CU_ASSERT_EQUAL(TEST_NUM, c.mag.a[0]);
	cbi_tocstr(&c, buf);
	CU_ASSERT_STRING_EQUAL("900000009", buf);

	for (int i=0; i<1000; i++) {
		cbi_add(&c, &c, &b);
		// TODO test every step?
	}

	cbi_tocstr(&c, buf);
	//printf("%s\n", buf);
	CU_ASSERT_STRING_EQUAL("900900009009", buf);

	cbi_setl(&a, 0);
	cbi_add(&c, &a, &b);
	cbi_tocstr(&c, buf);
	CU_ASSERT_STRING_EQUAL(xstr(TEST_NUM), buf);

	cbi_add(&c, &b, &a);
	cbi_tocstr(&c, buf);
	CU_ASSERT_STRING_EQUAL(xstr(TEST_NUM), buf);

	cbi_add(&c, &a, &a);
	cbi_tocstr(&c, buf);
	CU_ASSERT_STRING_EQUAL("0", buf);

	cbi_free(&a);
	cbi_free(&b);
	cbi_free(&c);
}

void sub_test()
{
	char buf[1000];
	cbigint a, b, c;
	cbi_init(&a);
	cbi_init(&b);
	cbi_init(&c);

	cbi_setl(&a, 12345);
	cbi_sub(&c, &a, &b);
	CU_ASSERT_STRING_EQUAL("12345", cbi_tocstr(&c, buf));

	cbi_sub(&c, &b, &a);
	CU_ASSERT_STRING_EQUAL("-12345", cbi_tocstr(&c, buf));

	cbi_sub(&c, &a, &a);
	CU_ASSERT_STRING_EQUAL("0", cbi_tocstr(&c, buf));

	cbi_setl(&b, 12346);
	cbi_sub(&c, &a, &b);
	CU_ASSERT_STRING_EQUAL("-1", cbi_tocstr(&c, buf));

	cbi_setl(&a, -90000);
	cbi_setl(&b, 100);
	cbi_sub(&c, &a, &b);
	CU_ASSERT_STRING_EQUAL("-90100", cbi_tocstr(&c, buf));

	cbi_setl(&b, -100);
	cbi_sub(&c, &a, &b);
	CU_ASSERT_STRING_EQUAL("-89900", cbi_tocstr(&c, buf));

	cbi_setl(&a, 90000);
	cbi_sub(&c, &a, &b);
	CU_ASSERT_STRING_EQUAL("90100", cbi_tocstr(&c, buf));

	cbi_free(&a);
	cbi_free(&b);
	cbi_free(&c);
}

void mult_test()
{
	char buf[1000];
	cbigint a, b, c;
	cbi_init(&a);
	cbi_init(&b);
	cbi_init(&c);

	cbi_setl(&a, 999);
	cbi_mult(&c, &a, &b);
	CU_ASSERT_STRING_EQUAL("0", cbi_tocstr(&c, buf));

	cbi_mult(&c, &b, &a);
	CU_ASSERT_STRING_EQUAL("0", cbi_tocstr(&c, buf));

	cbi_mult(&c, &a, &a);
	CU_ASSERT_STRING_EQUAL("998001", cbi_tocstr(&c, buf));

	cbi_setl(&b, 1);
	cbi_mult(&c, &a, &b);
	CU_ASSERT_STRING_EQUAL("999", cbi_tocstr(&c, buf));

	cbi_setl(&b, 1);
	cbi_mult(&c, &b, &a);
	CU_ASSERT_STRING_EQUAL("999", cbi_tocstr(&c, buf));

	cbi_setl(&a, -999);
	cbi_setl(&b, 100);
	cbi_mult(&c, &a, &b);
	CU_ASSERT_STRING_EQUAL("-99900", cbi_tocstr(&c, buf));

	cbi_setl(&b, -100);
	cbi_mult(&c, &a, &b);
	CU_ASSERT_STRING_EQUAL("99900", cbi_tocstr(&c, buf));

	cbi_free(&a);
	cbi_free(&b);
	cbi_free(&c);
}

void div_test()
{
	cbigint a = { 0 };
	cbigint b = { 0 };
	cbigint p = { 0 };
	cbigint r = { 0 };
	char buf[1024];
	
	cbi_setl(&a, 9900);
	cbi_setl(&b, 100);
	cbi_div(&p, &a, &b);
	cbi_tocstr(&p, buf);
	CU_ASSERT_STRING_EQUAL("99", buf);

	cbi_setl(&a, 1001);
	cbi_setl(&b, 99);
	cbi_div(&p, &a, &b);
	CU_ASSERT_STRING_EQUAL("10", cbi_tocstr(&p, buf));

	cbi_setl(&a, 1000);
	cbi_setl(&b, 10);
	cbi_div(&p, &a, &b);
	CU_ASSERT_STRING_EQUAL("100", cbi_tocstr(&p, buf));

	cbi_setl(&a, 1200);
	cbi_setl(&b, 10);
	cbi_div(&p, &a, &b);
	CU_ASSERT_STRING_EQUAL("120", cbi_tocstr(&p, buf));

	cbi_setl(&a, 3040000);
	cbi_setl(&b, 303);
	cbi_div(&p, &a, &b);
	CU_ASSERT_STRING_EQUAL("10033", cbi_tocstr(&p, buf));

	cbi_setl(&a, 3030000);
	cbi_setl(&b, 303);
	cbi_div(&p, &a, &b);
	CU_ASSERT_STRING_EQUAL("10000", cbi_tocstr(&p, buf));


	cbi_setl(&a, 9900);
	cbi_setl(&b, 100);
	cbi_mod(&p, &a, &b);
	cbi_tocstr(&p, buf);
	CU_ASSERT_STRING_EQUAL("0", buf);

	cbi_setl(&a, 1001);
	cbi_setl(&b, 99);
	cbi_mod(&p, &a, &b);
	CU_ASSERT_STRING_EQUAL("11", cbi_tocstr(&p, buf));

	cbi_setl(&a, 1000);
	cbi_setl(&b, 10);
	cbi_mod(&p, &a, &b);
	CU_ASSERT_STRING_EQUAL("0", cbi_tocstr(&p, buf));

	cbi_setl(&a, 1200);
	cbi_setl(&b, 467);
	cbi_mod(&p, &a, &b);
	CU_ASSERT_STRING_EQUAL("266", cbi_tocstr(&p, buf));

	cbi_setl(&a, 3040000);
	cbi_setl(&b, 303);
	cbi_mod(&p, &a, &b);
	CU_ASSERT_STRING_EQUAL("1", cbi_tocstr(&p, buf));

	cbi_setl(&a, 9876543212345);
	cbi_setl(&b, 37658);
	cbi_mod(&p, &a, &b);
	CU_ASSERT_STRING_EQUAL("639", cbi_tocstr(&p, buf));

	cbi_setl(&a, 9876543212345);
	cbi_setl(&b, 37658);
	cbi_divmod(&p, &a, &b, &r);
	CU_ASSERT_STRING_EQUAL("262269457", cbi_tocstr(&p, buf));
	CU_ASSERT_STRING_EQUAL("639", cbi_tocstr(&r, buf));

	cbi_free(&a);
	cbi_free(&b);
	cbi_free(&p);
	cbi_free(&r);
}

void pow_test()
{
	cbigint a, b;
	char buf[1024];

	cbi_init(&a);
	cbi_init(&b);

	cbi_setl(&a, 2);
	cbi_powl(&b, &a, 100);
	CU_ASSERT_STRING_EQUAL("1267650600228229401496703205376", cbi_tocstr(&b, buf));

	cbi_setl(&a, 3);
	cbi_powl(&b, &a, 71);
	CU_ASSERT_STRING_EQUAL("7509466514979724803946715958257547", cbi_tocstr(&b, buf));

	cbi_setl(&a, -2);
	cbi_powl(&b, &a, 100);
	CU_ASSERT_STRING_EQUAL("1267650600228229401496703205376", cbi_tocstr(&b, buf));

	cbi_setl(&a, -3);
	cbi_powl(&b, &a, 71);
	CU_ASSERT_STRING_EQUAL("-7509466514979724803946715958257547", cbi_tocstr(&b, buf));


	cbi_setl(&a, -1);
	cbi_powl(&b, &a, 100);
	CU_ASSERT_STRING_EQUAL("1", cbi_tocstr(&b, buf));

	cbi_setl(&a, -1);
	cbi_powl(&b, &a, 71);
	CU_ASSERT_STRING_EQUAL("-1", cbi_tocstr(&b, buf));

	cbi_setl(&a, 1);
	cbi_powl(&b, &a, 100);
	CU_ASSERT_STRING_EQUAL("1", cbi_tocstr(&b, buf));

	cbi_setl(&a, 1);
	cbi_powl(&b, &a, 71);
	CU_ASSERT_STRING_EQUAL("1", cbi_tocstr(&b, buf));

	cbi_setl(&a, 0);
	cbi_powl(&b, &a, 100);
	CU_ASSERT_STRING_EQUAL("0", cbi_tocstr(&b, buf));

	cbi_setl(&a, 0);
	cbi_powl(&b, &a, 71);
	CU_ASSERT_STRING_EQUAL("0", cbi_tocstr(&b, buf));

	cbi_free(&a);
	cbi_free(&b);
}

void addl_test()
{
	cbigint a = { 0 };

	//cbi_addl(

	cbi_free(&a);
}

void subl_test()
{
}

void multl_test()
{
}
void divl_test()
{
}


// general tests
void set_test()
{
	cbigint a = { 0 };
	cbigint b = { 0 };

	cbi_setl(&a, 12345);

	char buf[1000];
	cbi_tocstr(&a, buf);

	CU_ASSERT_STRING_EQUAL("12345", buf);

	long t = LONG_MAX - 100;
	cbi_setl(&a, 123456789012345678);

	cbi_setl(&b, 10000000000);
	cbi_add(&a, &a, &b);

	cbi_tocstr(&a, buf);
	CU_ASSERT_STRING_EQUAL("123456799012345678", buf);

	cbi_setl(&a, 0);
	cbi_add(&a, &a, &b);
	cbi_tocstr(&a, buf);
	CU_ASSERT_STRING_EQUAL("10000000000", buf);

	cbi_setl(&a, -10000000000);
	cbi_add(&a, &a, &b);
	cbi_tocstr(&a, buf);
	CU_ASSERT_STRING_EQUAL("0", buf);

	cbi_setl(&a, -9999999995);
	cbi_add(&a, &a, &b);
	cbi_tocstr(&a, buf);
	CU_ASSERT_STRING_EQUAL("5", buf);

	cbi_setl(&a, 100000000000);
	cbi_tocstr(&a, buf);
	CU_ASSERT_STRING_EQUAL("100000000000", buf);

	cbi_setl(&a, CBI_BASE);
	cbi_tocstr(&a, buf);
	CU_ASSERT_STRING_EQUAL(xstr(CBI_BASE), buf);
	CU_ASSERT_EQUAL(a.mag.size, 2);

	cbi_free(&a);
	cbi_free(&b);
}

void tol_test()
{
	cbigint a = { 0 };
	cbigint b;
	char buf[1000];
	long t = LONG_MAX - 100;

	cbi_setl(&a, 12345);

	sprintf(buf, "%ld", t);
	cbi_initfromcstr(&b, buf);

	long al = cbi_tol(&a);
	CU_ASSERT_EQUAL(al, 12345);

	long bl = cbi_tol(&b);
	CU_ASSERT_EQUAL(bl, t);

	cbi_setl(&a, -12345);

	sprintf(buf, "%ld", LONG_MIN);
	cbi_fromcstr(&b, buf);

	al = cbi_tol(&a);
	CU_ASSERT_EQUAL(al, -12345);

	bl = cbi_tol(&b);
	CU_ASSERT_EQUAL(bl, LONG_MIN);

	cbi_free(&a);
	cbi_free(&b);
}

void fromcstr_test()
{
	// TODO both varieties
}

void compare_test()
{
	// TODO both varieties
}


void arithmetic_test()
{
	char filename[100];

	char* num_buf = malloc(1 << 20);
	//char num_buf[1024];

	FILE* fin;
	FILE* fout = stdout;

	//char three[] = "3";
	//char two[] = "2";

	cbigint a, b, c, tmp1, tmp2;
	cbi_init(&a);
	cbi_init(&b);
	cbi_init(&c);
	cbi_init(&tmp1);
	cbi_init(&tmp2);

	int n_files = 5;

	for (int i=5; i<=n_files; ++i) {
		snprintf(filename, 100, "./in%d", i);

		//printf("'%s'\n", filename);

		if (!(fin = fopen(filename, "r"))) {
			perror("failed to open file");
			assert(fin);
		}

		cbi_read(&a, fin);
		cbi_read(&b, fin);

		fprintf(fout, "%s\n\n", cbi_tocstr(&a, num_buf));
		fprintf(fout, "%s\n\n", cbi_tocstr(&b, num_buf));

		cbi_add(&c, &a, &b);
		fprintf(fout, "%s\n\n", cbi_tocstr(&c, num_buf));

		cbi_sub(&c, &a, &b);
		fprintf(fout, "%s\n\n", cbi_tocstr(&c, num_buf));

		cbi_sub(&c, &a, &a);
		fprintf(fout, "%s\n\n", cbi_tocstr(&c, num_buf));

		cbi_setl(&tmp1, 3);
		cbi_mult(&tmp1, &a, &tmp1);

		cbi_setl(&tmp2, 2);
		cbi_mult(&tmp2, &tmp2, &b);

		cbi_sub(&c, &tmp1, &tmp2);
		fprintf(fout, "%s\n\n", cbi_tocstr(&c, num_buf));

		cbi_mult(&c, &a, &b);
		fprintf(fout, "%s\n\n", cbi_tocstr(&c, num_buf));

		cbi_mult(&tmp1, &a, &a);
		fprintf(fout, "%s\n\n", cbi_tocstr(&tmp1, num_buf));

		cbi_mult(&tmp2, &b, &b);
		fprintf(fout, "%s\n\n", cbi_tocstr(&tmp2, num_buf));

		// a^4
		cbi_mult(&tmp1, &tmp1, &tmp1);

		cbi_setl(&c, 9);
		cbi_mult(&tmp1, &tmp1, &c);

		// b^5
		cbi_mult(&tmp2, cbi_mult(&tmp2, &tmp2, &tmp2), &b);
		cbi_setl(&c, 16);
		cbi_mult(&tmp2, &tmp2, &c);

		// 9a^4 + 16b^5
		cbi_add(&c, &tmp1, &tmp2);
		fprintf(fout, "%s\n\n", cbi_tocstr(&c, num_buf));

		fclose(fin);
	}

	free(num_buf);
	cbi_free(&a);
	cbi_free(&b);
	cbi_free(&c);
	cbi_free(&tmp1);
	cbi_free(&tmp2);

}


char* freadline(FILE* input)
{
	return freadstring(input, '\n', 0);
}

/** Reads and returns a newly allocated string from file.  It reads
 * until delim is reached or max_len characters are read.  Delim is not
 * included in the string.  max_len refers to the resulting strlen, and
 * the string is always null terminated.  If max_len is 0, freadstring
 * will continue to read, reallocated as necessary, until delim is hit
 * or allocation fails */
char* freadstring(FILE* input, int delim, size_t max_len)
{
	char* string = NULL, *tmp_str = NULL;
	int temp;
	int i=0;
	int inf = 0;

	if (feof(input))
		return NULL;

	if (!max_len) {
		inf = 1;
		max_len = 4096;
	}

	if(!(string = (char*)malloc(max_len+1)))
		return NULL;

	while (1) {
		temp = fgetc(input);

		if (temp == EOF || temp == delim) {
			if (!i && temp != delim) { //allow for delim == EOF
				free(string);
				return NULL;
			}
			tmp_str = (char*)realloc(string, i+1);
			if (!tmp_str) {
				free(string);
				return NULL;
			}
			string = tmp_str;
			break;
		}

		if (i == max_len) {
			if (inf) {
				tmp_str = (char*)realloc(string, max_len*2+1);
				if (!tmp_str) {
					free(string);
					return NULL;
				}
				string = tmp_str;
				// TODO fix in c_utils
				max_len *= 2;
			} else {
				break;
			}
		}

		string[i] = temp;
		i++;
	}
	string[i] = '\0';


	return string;
}











