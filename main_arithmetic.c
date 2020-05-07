

#include "cbigint.h"

#include <stdio.h>


int main(int argc, char** argv)
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
			return 0;
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

	return 0;
}
