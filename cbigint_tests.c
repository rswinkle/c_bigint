
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

	cbi_set(&b, TEST_NUM);

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


	cbi_free(&a);
	cbi_free(&b);
	cbi_free(&c);
}

void set_test()
{
	cbigint a = { 0 };
	cbigint b = { 0 };

	cbi_set(&a, 12345);

	char buf[1000];
	cbi_tocstr(&a, buf);

	CU_ASSERT_STRING_EQUAL("12345", buf);

	long t = LONG_MAX - 100;
	cbi_set(&a, 123456789012345678);

	cbi_set(&b, 10000000000);
	cbi_add(&a, &a, &b);

	cbi_tocstr(&a, buf);
	CU_ASSERT_STRING_EQUAL("123456799012345678", buf);

	cbi_set(&a, 0);
	cbi_add(&a, &a, &b);
	cbi_tocstr(&a, buf);
	CU_ASSERT_STRING_EQUAL("10000000000", buf);

	cbi_set(&a, -10000000000);
	cbi_add(&a, &a, &b);
	cbi_tocstr(&a, buf);
	CU_ASSERT_STRING_EQUAL("0", buf);

	cbi_set(&a, -9999999995);
	cbi_add(&a, &a, &b);
	cbi_tocstr(&a, buf);
	CU_ASSERT_STRING_EQUAL("5", buf);

	cbi_set(&a, 100000000000);
	cbi_tocstr(&a, buf);
	CU_ASSERT_STRING_EQUAL("100000000000", buf);

	cbi_set(&a, CBI_BASE);
	cbi_tocstr(&a, buf);
	CU_ASSERT_STRING_EQUAL(xstr(CBI_BASE), buf);
	CU_ASSERT_EQUAL(a.mag.size, 2);

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

void sub_test()
{

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

		cbi_set(&tmp1, 3);
		cbi_mult(&tmp1, &a, &tmp1);

		cbi_set(&tmp2, 2);
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

		cbi_set(&c, 9);
		cbi_mult(&tmp1, &tmp1, &c);

		// b^5
		cbi_mult(&tmp2, cbi_mult(&tmp2, &tmp2, &tmp2), &b);
		cbi_set(&c, 16);
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











