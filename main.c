
#include "cbigint_tests.c"

#include <stdio.h>
#include <CUnit/Automated.h>


extern void add_test();
extern void sub_test();




CU_TestInfo op_tests[] = {
	{ "add_test",           add_test },
	{ "sub_test",           sub_test },
	{ "mult_test",          mult_test },
	{ "div_test",           div_test },
	{ "pow_test",           pow_test },
	CU_TEST_INFO_NULL
};


CU_TestInfo general_tests[] = {
//	{ "arithmetic",        arithmetic_test },
	{ "cbi_set",           set_test },
	CU_TEST_INFO_NULL
};





CU_SuiteInfo cbigint_suites[] = {
#ifndef OLD_CUNIT
	{ "op_tests",     NULL, NULL, NULL, NULL, op_tests },
	{ "general_tests",     NULL, NULL, NULL, NULL, general_tests },
#else
	{ "add_tests",     NULL, NULL, add_tests },
	{ "sub_tests",     NULL, NULL, sub_tests },
	{ "general_tests",     NULL, NULL, general_tests },
#endif
	CU_SUITE_INFO_NULL
};



int main()
{
	CU_ErrorCode error;
	error = CU_initialize_registry();
	if (error != CUE_SUCCESS) {
		fprintf(stderr, "Failed to initialize registry\n");
		return CU_get_error();
	}

	error = CU_register_suites(cbigint_suites);

	if (error != CUE_SUCCESS) {
		fprintf(stderr, "Failed to register test suite\n");
		return CU_get_error();
	}

	CU_automated_run_tests();
	fprintf(stdout, "CU_get_error() returned %d\n", CU_get_error());

	CU_cleanup_registry();

	return CU_get_error();

}
