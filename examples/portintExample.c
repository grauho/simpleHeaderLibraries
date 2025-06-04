#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PORTINT_VERIFY_VARIABLE_SIZES
#include "../portint.h"

#if !defined(MY_SYSTEM_WIDTH) && ((SIZE_MAX >> 15) == 1)
#define MY_SYSTEM_WIDTH 16
#endif
#if !defined(MY_SYSTEM_WIDTH) && ((SIZE_MAX >> 31) == 1)
#define MY_SYSTEM_WIDTH 32
#endif
#if !defined(MY_SYSTEM_WIDTH) && ((SIZE_MAX >> 63) == 1)
#define MY_SYSTEM_WIDTH 64
#endif

#if !defined(MY_SYSTEM_WIDTH)
#error "Unable to guess the systems address width, define explicitly"
#endif

int main(int argc, char **argv)
{
	fprintf(stdout, "You are using a %d-bit system\n", MY_SYSTEM_WIDTH);
	logPortintStatus();

	return 0;
}

