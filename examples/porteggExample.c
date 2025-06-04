/* portegg test file */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#include "../portegg.h"

#define RAWBYTE_INI "Why do they eat their eggs like that?"
#define UINT16_INI UINT16_MAX - 42
#define UINT32_INI UINT32_MAX - 123
#define UINT64_INI UINT64_MAX - 112358
#define FLOAT_INI  234.123412
#define DOUBLE_INI 0.000213412

int main(int argc, char **argv)
{
	uint16_t test_16 = UINT16_INI;
	uint32_t test_32 = UINT32_INI;
	uint64_t test_64 = UINT64_INI;
	float test_flt   = FLOAT_INI;
	double test_dbl  = DOUBLE_INI;
	char test_str[]  = RAWBYTE_INI;
	const size_t str_len = sizeof(test_str) - 1;

	porteggSysToLeCopy(uint16_t, test_16,  test_16);
	porteggSysToLeCopy(uint32_t, test_32,  test_32);
	porteggSysToLeCopy(uint64_t, test_64,  test_64);
	porteggSysToLeCopy(float,    test_flt, test_flt);
	porteggSysToLeCopy(double,   test_dbl, test_dbl);
	PORTEGG_SYS_TO_LE_RAW(str_len, test_str);

	fprintf(stdout, "Little-endian:\n16-bit: %u 32-bit: %u 64-bit: %lu\n"
		"flt-32: %f\nflt-64: %lf\nstr: %s\n\n", 
		test_16, test_32, test_64, test_flt, test_dbl, test_str);

	porteggLeToSysCopy(uint16_t, test_16,  test_16);
	porteggLeToSysCopy(uint32_t, test_32,  test_32);
	porteggLeToSysCopy(uint64_t, test_64,  test_64);
	porteggLeToSysCopy(float,    test_flt, test_flt);
	porteggLeToSysCopy(double,   test_dbl, test_dbl);
	PORTEGG_LE_TO_SYS_RAW(str_len, test_str);

	fprintf(stdout, "System:\n16-bit: %u 32-bit: %u 64-bit: %lu\n"
		"flt-32: %f\nflt-64: %lf\nstr: %s\n\n",
		test_16, test_32, test_64, test_flt, test_dbl, test_str);

	porteggSysToBeCopy(uint16_t, test_16,  test_16);
	porteggSysToBeCopy(uint32_t, test_32,  test_32);
	porteggSysToBeCopy(uint64_t, test_64,  test_64);
	porteggSysToBeCopy(float,    test_flt, test_flt);
	porteggSysToBeCopy(double,   test_dbl, test_dbl);
	PORTEGG_SYS_TO_BE_RAW(str_len, test_str);

	fprintf(stdout, "Big-endian:\n16-bit: %u 32-bit: %u 64-bit: %lu\n"
		"flt-32: %f\nflt-64: %lf\nstr: %s\n\n", 
		test_16, test_32, test_64, test_flt, test_dbl, test_str);

	porteggBeToSysCopy(uint16_t, test_16,  test_16);
	porteggBeToSysCopy(uint32_t, test_32,  test_32);
	porteggBeToSysCopy(uint64_t, test_64,  test_64);
	porteggBeToSysCopy(float,    test_flt, test_flt);
	porteggBeToSysCopy(double,   test_dbl, test_dbl);
	PORTEGG_BE_TO_SYS_RAW(str_len, test_str);

	fprintf(stdout, "System:\n16-bit: %u 32-bit: %u 64-bit: %lu\n"
		"flt-32: %f\nflt-64: %lf\nstr: %s\n\n",
		test_16, test_32, test_64, test_flt, test_dbl, test_str);
	
	fprintf(stdout, "16-bits:%7s\n32-bits:%7s\n64-bits:%7s\n32-flt:%8s\n"
		"64-flt:%8s\nraw-bytes:%5s\n",
		(test_16  == UINT16_INI) ? "PASS" : "FAIL",
		(test_32  == UINT32_INI) ? "PASS" : "FAIL",
		(test_64  == UINT64_INI) ? "PASS" : "FAIL",
		(test_flt == (float) FLOAT_INI)  ? "PASS" : "FAIL",
		(test_dbl == DOUBLE_INI) ? "PASS" : "FAIL",
		(strcmp(test_str, RAWBYTE_INI) == 0) ? "PASS" : "FAIL");

	return (test_16 == UINT16_INI)
		&& (test_32 == UINT32_INI)
		&& (test_64 == UINT64_INI)
		&& (test_flt == (float) FLOAT_INI)
		&& (test_dbl == DOUBLE_INI)
		&& (strcmp(test_str, RAWBYTE_INI) == 0);
}

