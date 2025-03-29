/* License information at EOF */
/* A platform independant bitfield to circumvent the platform defined bitfield 
 * behavior in C90 */

/* API will be composed of functions to allocate and free a bitfield of an
 * arbitrary length as well as read an arbitrary length sequence at an 
 * arbitrary offset into the bitfield */

 /* Bit access is zero based just like array access */

/* I wonder if there is a way to provide these bitfields in such a way that if 
 * a structure containing one of these bitfields when written out to a file or
 * sent over a socket to a machine with a different architecture will still 
 * allow the recieving architecture to use them, otherwise if this will only 
 * work when using the bitfield on a system that the code was compiled on it
 * isn't significantly more useful than the current platform dependant
 * bitfields. I think the mose sane approach is just settle on one endianess
 * and let the user know what to expect if they're using a difference 
 * architecture */
#ifndef PORTBIT_H
#define PORTBIT_H

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#else
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
#endif

#ifndef PORTBIT_CALLOC
#include <stdlib.h> 
#define PORTBIT_CALLOC calloc
#endif /* ifndef PORTBIT_CALLOC */

/* Limits can be replaced with a little bit of work but even freestanding envs 
 * are supposed to have limits.h */
#include <limits.h>

#define PORTBIT_API	static
#define PORTBIT_BOOL    char 
#define PORTBIT_STAT    char
#define PORTBIT_TRUE    (1)
#define PORTBIT_FALSE   (0)
#define PORTBIT_ON      PORTBIT_TRUE
#define PORTBIT_OFF     PORTBIT_FALSE
#define PORTBIT_SUCCESS (0)
#define PORTBIT_FAILURE (1)

/* Marco to get which byte in the bitfield the corresponding bit position will
 * be contained within, bear in mind this returns the zero based index. I 
 * doubt this will be run on a non-POSIX compliant system that uses a different
 * byte width but you never know. PORTBIT_BYTE_TO_BIT is mostly just included for 
 * completeness, there isn't much need to opitmize multiplication */
#if CHAR_BIT != 8
#define PORTBIT_BIT_TO_BYTE(pos) ((pos) / CHAR_BIT)
#define PORTBIT_BYTE_TO_BIT(pos) ((pos) * CHAR_BIT)
#else
#define PORTBIT_BIT_TO_BYTE(pos) ((pos) >> 3)
#define PORTBIT_BYTE_TO_BIT(pos) ((pos) << 3)
#endif /* CHAR_BIT != 8 */

#define PORTBIT_BITSIZE(type) (sizeof(type) * CHAR_BIT)

#define PORTBIT_BIT_TO_FIELD_LEN(num_bits) \
	(PORTBIT_BIT_TO_BYTE((num_bits)) + ((num_bits) % CHAR_BIT != 0))

/* Only works on bytes, to use on a given type one would have to also pass that
 * in so CHAR_BIT can be replaced with PORTBIT_BITSIZE(type) */
#define PORTBIT_IS_BIT_ON(byte, bit_pos)                \
	((((byte) >> ((CHAR_BIT - 1) - (bit_pos))) & 1) \
		? PORTBIT_TRUE : PORTBIT_FALSE)

/* This function is only for setting bits in a bitfield, setting bits in a 
 * type that is being read out from the bitfield is done similarly but is
 * bespoke to the type down in readBitsIntoTYPE down below */
#define PORTBIT_SET_BIT(data, byte, bit, state)                   \
do                                                                \
{                                                                 \
	if ((state) == PORTBIT_ON)                                \
	{                                                         \
		(data)[(byte)] |= (unsigned char)                 \
				(1 << ((CHAR_BIT - 1) - (bit)));  \
	}                                                         \
	else                                                      \
	{                                                         \
		(data)[(byte)] &= (unsigned char)                 \
				~(1 << ((CHAR_BIT - 1) - (bit))); \
	}                                                         \
} while(0)

/* The question that remains is whether or not it is worth also allowing
 * for an offset of bits into the type being read from */
#define PORTBIT_GENERATE_WRITE_FUNCTION(name, type)                          \
PORTBIT_API PORTBIT_STAT writeBitsFrom##name(const type src,                 \
	const size_t num, const size_t pos, struct bitfield *dest)           \
{                                                                            \
	size_t i, byte, bit;                                                 \
	                                                                     \
	if ((dest == NULL)                                                   \
	|| (PORTBIT_BYTE_TO_BIT(dest->length) < pos + num)                   \
	|| (num > PORTBIT_BITSIZE(type)))                                    \
	{                                                                    \
		return PORTBIT_FAILURE;                                      \
	}                                                                    \
	                                                                     \
        byte = PORTBIT_BIT_TO_BYTE(pos);                                     \
	bit  = pos % CHAR_BIT;                                               \
	                                                                     \
	for (i = 0; i < num; i++)                                            \
	{                                                                    \
		if ((src >> ((PORTBIT_BITSIZE(type) - 1) - (i))) & 1)        \
		{                                                            \
			PORTBIT_SET_BIT(dest->data, byte, bit, PORTBIT_ON);  \
		}                                                            \
		else                                                         \
		{                                                            \
			PORTBIT_SET_BIT(dest->data, byte, bit, PORTBIT_OFF); \
		}                                                            \
		                                                             \
		bit++;                                                       \
		                                                             \
		if (bit == CHAR_BIT)                                         \
		{                                                            \
			bit = 0;                                             \
			byte++;                                              \
		}                                                            \
	}                                                                    \
	                                                                     \
	return PORTBIT_SUCCESS;                                              \
} 

/* This will work for non-floating integer types, for those some special 
 * handling will be required. This function crawls the bitfield to try to avoid
 * repeated calls to getBitState which would require repeated and unneeded 
 * modulo arithmatic */
/* TODO: Might need special handling for the sign bit */
#define PORTBIT_GENERATE_READ_FUNCTION(name, type)                           \
PORTBIT_API PORTBIT_STAT readBitsInto##name(const struct bitfield *src,      \
	const size_t num, const size_t pos, type *dest)                      \
{                                                                            \
	size_t i, byte, bit;                                                 \
                                                                             \
	if ((src == NULL) || (dest == NULL)                                  \
	|| (PORTBIT_BYTE_TO_BIT(src->length) < pos + num)                    \
	|| (num > PORTBIT_BITSIZE(type)))                                    \
	{                                                                    \
		return PORTBIT_FAILURE;                                      \
	}                                                                    \
	                                                                     \
	byte = PORTBIT_BIT_TO_BYTE(pos);                                     \
	bit  = pos % CHAR_BIT;                                               \
	                                                                     \
	for (i = 0; i < num; i++)                                            \
	{                                                                    \
		if (PORTBIT_IS_BIT_ON(src->data[byte], bit) == PORTBIT_TRUE) \
		{                                                            \
			(*dest) |= (type) (1 << (num - i - 1));              \
		}                                                            \
		else                                                         \
		{                                                            \
			(*dest) &= (type) ~(1 << (num - i - 1));             \
		}                                                            \
		                                                             \
		bit++;                                                       \
		                                                             \
		if (bit == CHAR_BIT)                                         \
		{                                                            \
		        bit = 0;                                             \
		        byte++;                                              \
		}                                                            \
	}                                                                    \
	                                                                     \
	return PORTBIT_SUCCESS;                                              \
}

#define PORTBIT_GENERATE_BOTH_FUNCTIONS(name, type) \
PORTBIT_GENERATE_WRITE_FUNCTION(name, type)         \
PORTBIT_GENERATE_READ_FUNCTION(name, type)

#define PORTBIT_AND_THE_KITCHEN_SINK                \
PORTBIT_GENERATE_BOTH_FUNCTIONS(Char, char)         \
PORTBIT_GENERATE_BOTH_FUNCTIONS(Short, short int)   \
PORTBIT_GENERATE_BOTH_FUNCTIONS(Integer, int)       \
PORTBIT_GENERATE_BOTH_FUNCTIONS(Long, long int)

const char test_str[] = "110101011100111101111"; 
const size_t test_len = (sizeof(test_str) / sizeof(test_str[0])) - 1;

struct bitfield
{
	unsigned char *data;
	size_t length;
};

PORTBIT_API struct bitfield* newBitfield(const size_t bit_length)
{
	struct bitfield *field = PORTBIT_CALLOC(1, sizeof(struct bitfield));

	if (field == NULL)
	{
		return NULL;
	}

	field->length = PORTBIT_BIT_TO_FIELD_LEN(bit_length);

	if ((field->data = PORTBIT_CALLOC(field->length, 
		sizeof(unsigned char))) == NULL)
	{
		free(field);

		return NULL;
	}

	return field;
}

PORTBIT_API void freeBitfield(struct bitfield *field)
{
	if (field != NULL)
	{
		if (field->data != NULL)
		{
			free(field->data);
			field->data = NULL;
		}

		free(field);
		field = NULL;
	}
}

/* This version might be preferable because it allows one to crawl the 
 * bitfield more easily because it abstracts away the need to care about bytes
 * and instead can just be incrimented, although it might be good to be able
 * to return an error. Also, there is room for a more optimized access if the
 * number of bits requested is known ahead of time where the modulo 
 * arithmatic and division can be done only once at the start regardless of 
 * how many bytes the request spans, this is likely what is be done in the
 * readBitsInto$TYPE functions generated from the corresponding macro */
PORTBIT_API PORTBIT_BOOL getBitState(const struct bitfield *field, 
	const size_t pos)
{
	if (pos > PORTBIT_BYTE_TO_BIT(field->length))
	{
		return PORTBIT_FALSE;
	}

	return PORTBIT_IS_BIT_ON(field->data[PORTBIT_BIT_TO_BYTE(pos)], 
			pos % CHAR_BIT);
}

/* Do the check for OOB access here, return PORTBIT_FAILURE if OOB */
PORTBIT_API PORTBIT_STAT setBit(struct bitfield *field, const size_t bit_pos, 
	const PORTBIT_BOOL state)
{
	if (PORTBIT_BIT_TO_BYTE(bit_pos) >= field->length)
	{
		return PORTBIT_FAILURE;
	}

	PORTBIT_SET_BIT(field->data, PORTBIT_BIT_TO_BYTE(bit_pos), 
		bit_pos % CHAR_BIT, state);

	return PORTBIT_SUCCESS;
}

PORTBIT_API PORTBIT_STAT setBitsFromString(struct bitfield *field, 
	const char *val_str, const size_t offset)
{
	size_t i;

	for (i = 0; val_str[i] != '\0'; i++)
	{
		switch (val_str[i])
		{
			case '1':
				if (setBit(field, i + offset, PORTBIT_ON) 
					== PORTBIT_FAILURE)
				{
					return PORTBIT_FAILURE;
				}

				break;
			case '0':
				if (setBit(field, i + offset, PORTBIT_OFF) 
					== PORTBIT_FAILURE)
				{
					return PORTBIT_FAILURE;
				}

				break;
			default:
				return PORTBIT_FAILURE;
		}
	}

	return PORTBIT_SUCCESS;
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#else
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#endif

#endif /* PORTBIT_H */

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
