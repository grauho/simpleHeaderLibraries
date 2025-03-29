/* License information at EOF */
#ifndef PORTINT_H 
#define PORTINT_H

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#else
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
#endif

/* This file is meant as a stopgap replacement for stdint.h least*_t integer
 * types, one should use stdint.h if one can */

#include <limits.h>

#if ((UCHAR_MAX >> 7) >= 1)
#define p_S8 signed char
#define p_U8 unsigned char
#else /* Non-POSIX compliant I see! */
#if ((USHRT_MAX >> 7) >= 1)
#define p_S8 signed short int
#define p_U8 unsigned short int
#else /* If you get to here something is seriously strange with your system */
#if ((UINT_MAX >> 7) >= 1)
#define p_S8 signed int
#define p_U8 unsigned int
#else
#if ((ULONG_MAX >> 7) >= 1)
#define p_S8 signed long int
#define p_U8 unsigned long int
#else
#error "No 8-bit integer type found in ANSI C90 standard compliant types!"
#endif
#endif
#endif
#endif /* 8-bit check */

#if ((USHRT_MAX >> 15) >= 1)
#define p_S16 signed short int
#define p_U16 unsigned short int
#else
#if ((UINT_MAX >> 15) >= 1)
#define p_S16 signed int
#define p_U16 unsigned int
#else
#if ((ULONG_MAX >> 15) >= 1)
#define p_S16 signed long
#define p_U16 unsigned long
#else
#error "No 16-bit integer type found in ANSI C90 standard compliant types!"
#endif
#endif
#endif /* 16-bit check */

#if ((UINT_MAX >> 31) >= 1)
#define p_S32 signed int
#define p_U32 unsigned int
#else
#if ((ULONG_MAX >> 31) >= 1)
#define p_S32 signed long
#define p_U32 unsigned long
#else
#error "No 32-bit integer type found in ANSI C90 standard compliant types!"
#endif
#endif /* 32-bit check */

#if ((ULONG_MAX >> 63) >= 1)
#define p_S64 signed long int
#define p_U64 unsigned long int
#define PORTINT_INT64_AVAILABLE
#define PORTINT_HAS_INT64 1
#else /* Base long check */
#if defined(PORTINT_ALLOW_EXTENSIONS) && ((ULLONG_MAX >> 63) >= 1)
#define p_S64 signed long long int
#define p_U64 unsigned long long int
#define PORTINT_INT64_AVAILABLE
#define PORTINT_HAS_INT64 1
#else /* ie: if ulong == 32-bit and compiler extensions are not allowed */
#define PORTINT_INT64_UNAVAILABLE
#define PORTINT_HAS_INT64 0
#endif /* ext checks */
#endif /* 64-bit check */

#ifdef PORTINT_VERIFY_VARIABLE_SIZES

#include <stdio.h>
#if PORTINT_HAS_INT64
#define PORTINT_ESC64_SYM "%ld"
#define PORTINT_ESC64_VAL (sizeof(p_U64))
#else
#define PORTINT_ESC64_SYM "%s"
#define PORTINT_ESC64_VAL "Unavailable"
#endif

enum portintStatus
{
	PORTINT_OKAY = 0,
	PORTINT_INT08_OVER = (1 << 0),
	PORTINT_INT16_OVER = (1 << 1),
	PORTINT_INT32_OVER = (1 << 2),
	PORTINT_INT64_OVER = (1 << 3),
	PORTINT_INT64_MISS = (1 << 4),
};

static unsigned char getPortintStatus(void)
{
	unsigned char ret = PORTINT_OKAY;

	if (sizeof(p_U8) > 1)
	{
		ret |= PORTINT_INT_08_OVER;
	}

	if (sizeof(p_U16) > 2)
	{
		ret |= PORTINT_INT16_OVER;
	}

	if (sizeof(p_U32) > 4)
	{
		ret |= PORTINT_INT32_OVER;
	}
#ifdef PORTINT_INT64_AVAILABLE
	if (sizeof(p_U64) > 4)
	{
		ret |= PORTINT_INT64_OVER;
	}
#else
	ret |= PORTINT_INT64_OVER;
	ret |= PORTINT_INT64_MISS;
#endif

	return ret;
}

static void logPortintStatus(void)
{
	fprintf(stderr, 
		"p_U8  = %ld bytes\t%s\n"
		"p_U16 = %ld bytes\t%s\n"
		"p_U32 = %ld bytes\t%s\n"
		"p_U64 = "PORTINT_ESC64_SYM" bytes\t%s\n", 
		sizeof(p_U8),  
			(sizeof(p_U8)  == 1) 
				? "\033[32;1m[EXACT]\033[0m" 
				: "\033[33;1m[OVER]\033[0m",
		sizeof(p_U16), 
			(sizeof(p_U16) == 2) 
				? "\033[32;1m[EXACT]\033[0m" 
				: "\033[33;1m[OVER]\033[0m",
		sizeof(p_U32), 
			(sizeof(p_U32) == 4) 
				? "\033[32;1m[EXACT]\033[0m" 
				: "\033[33;1m[OVER]\033[0m",
		PORTINT_ESC64_VAL, 
#ifdef PORTINT_INT64_AVAILABLE
			(sizeof(p_U64) == 8) 
				? "\033[32;1m[EXACT]\033[0m" 
				: "\033[33;1m[OVER]\033[0m" 
#else
			"\033[31;1m[MISSING]\033[0m"
#endif
		);
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#else
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#endif

#endif /* VERIFY_PORTINT_VARIABLE_SIZES */
#endif /* PORTINT_H */

/*
BSD 4-Clause License
Copyright (c) 2024, grauho <grauho@proton.me> All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution.

    All advertising materials mentioning features or use of this software must 
    display the following acknowledgement: This product includes software 
    developed by the <copyright holder>.

    Neither the name of the <copyright holder> nor the names of its 
    contributors may be used to endorse or promote products derived from this 
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> AS IS AND ANY EXPRESS OR 
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO 
EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
