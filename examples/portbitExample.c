#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

#include "../portbit.h"

PORTBIT_GENERATE_BOTH_FUNCTIONS(Header, uint64_t)
PORTBIT_GENERATE_BOTH_FUNCTIONS(Byte, unsigned char)

/* Ends up using 24 bytes, the implimentation that motivated me to make this
 * uses 26 by using fancy things like Huffman encoding while I have surpassed 
 * it with mere bit-fiddling */
#define CHESS_STATE_BIT_LEN 192

/* The boolean atlas that represents which squares on the board are occupied
 * takes up 64 bits, or eight bytes. Thus, if one were so inclined, requires
 * only a single read from the bitfield, the benefit of storing the "atlas" as
 * a header to the bitfield is that with a simple bitwise operation one can
 * observe changes to the boardstate that will then be borne out by the 
 * following piece data. One question however is how to quickly index into
 * the piece data to find which pieces in question are identified as having
 * changed by the aformentioned bitwise operation */

/* One option would be to have the size of the serialized boardstate remain
 * unchanged regardless of how many pieces remain on the board. This would 
 * result in a lot of wasted space when serializing serveral boardstates but
 * would also allow for simpler indexing by bitwise operation */
#define BOARD_ATLAS_BIT_LEN 64

const char initial_board[] = 
	"R H B Q K B H R"
	"P P P P P P P P"
	"_ _ _ _ _ _ _ _"
	"_ _ _ _ _ _ _ _"
	"_ _ _ _ _ _ _ _"
	"_ _ _ _ _ _ _ _"
	"p p p p p p p p"
	"r h b q k b h r";

const char kasp_immortal[] = 
	"_ _ _ R _ _ _ R"
	"_ r _ _ _ P _ P"
	"_ _ _ _ _ _ P _"
	"_ p _ _ _ _ _ _"
	"_ _ Q _ _ _ _ _"
	"_ _ _ _ _ p p b"
	"_ q _ _ _ _ _ p"
	"_ k _ K _ _ _ _";

const size_t kasp_size = sizeof(kasp_immortal) / sizeof(kasp_immortal[0]);

const unsigned char piece_table[] =
{
	'P', 0x00, /* Pawn */
	'R', 0x01, /* Rook */
	'H', 0x02, /* Knight */
	'B', 0x03, /* Bishop */
	'Q', 0x04, /* Queen */
	'K', 0x05, /* King */
	/* I've been scooped, just a shared piece_id can denote both
	 * a vulnerable pawn and a castle-able rook because they'll
	 * never be on the same row. that way the remaining slot can
	 * be used for a king variation to denote whose turn it is */
	'V', 0x06, /* UNUSED, vulnerable pawn to en-passent */
	'C', 0x07  /* UNUSED, rook still eligable to castle */
};
const size_t table_len = sizeof(piece_table) / sizeof(piece_table[0]);

static void dumpBitfield(const struct bitfield *field)
{
	size_t i;

	for (i = 0; i < PORTBIT_BYTE_TO_BIT(field->length); i++)
	{
		if (i % CHAR_BIT == 0)
		{
			fprintf(stdout, "\nByte %-3ld: ", 
				PORTBIT_BIT_TO_BYTE(i));
		}

		fputs((getBitState(field, i) == PORTBIT_ON) 
			? "1 " : "0 ", stdout);
	}

	fputc('\n', stdout);

	return;
}

static unsigned char encodePiece(const char raw_piece)
{
	/* 1101 1111 */
	const unsigned char lowercase_mask = 0xDF;
	unsigned char encoded_piece = 0xFF;
	size_t i = 0;

	for (i = 0; i < table_len; i += 2)
	{
		if (piece_table[i] == (raw_piece & lowercase_mask))
		{
			encoded_piece = piece_table[i + 1];

			if ((raw_piece & ~lowercase_mask) != 0)
			{
				encoded_piece |= 1 << 3;
			}
		}
	}

	return encoded_piece;
}

static PORTBIT_STAT serializeBoard(const char *board_state, 
	struct bitfield *field)
{
	size_t atlas_bit = 0;
	size_t piece_bit = BOARD_ATLAS_BIT_LEN;
	size_t i = 0;

	/* 1101 1111 */
	const unsigned char lowercase_mask = 0xDF;

	if ((board_state == NULL) || (field == NULL))
	{
		return PORTBIT_FAILURE;
	}

	while (board_state[i] != '\0')
	{
		const unsigned char piece_id = encodePiece(board_state[i]);

		switch (board_state[i] & lowercase_mask)
		{
			case 'R': /* fallthrough */
			case 'H': /* fallthrough */
			case 'B': /* fallthrough */
			case 'Q': /* fallthrough */
			case 'K': /* fallthrough */
			case 'P': 
				writeBitsFromByte(piece_id << 4, 4, piece_bit, 
						field);
				piece_bit += 4;

			case '_': 
				writeBitsFromByte((piece_id == 0xFF) 
					? PORTBIT_FALSE : PORTBIT_TRUE << 7, 
					1, atlas_bit, field);
				atlas_bit += 1;

				break;

			default:
				break;
		}

		i++;
	}

	return PORTBIT_SUCCESS;
}

const unsigned char decodePiece(const unsigned char code)
{
	size_t i;

	for (i = 1; i < table_len; i += 2)
	{
		/* Mask off the piece color and compare */
		if ((code & 0x07) == piece_table[i])
		{
			return piece_table[i - 1];
		}
	}

	return '?';
}

enum ansiColor
{
	ANSI_BLACK = 30,
	ANSI_RED,
	ANSI_GREEN,
	ANSI_YELLOW,
	ANSI_BLUE,
	ANSI_MAGENTA,
	ANSI_CYAN,
	ANSI_WHITE,
	ANSI_BRIGHT_BLACK = 90,
	ANSI_BRIGHT_RED,
	ANSI_BRIGHT_GREEN,
	ANSI_BRIGHT_YELLOW,
	ANSI_BRIGHT_BLUE,
	ANSI_BRIGHT_MAGENTA,
	ANSI_BRIGHT_CYAN,
	ANSI_BRIGHT_WHITE
};

static void clrPrintf(const enum ansiColor fnt, const enum ansiColor bkg,
	const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stdout, "\033[%d;%dm", fnt, bkg + 10);
	vfprintf(stdout, fmt, args);
	fprintf(stdout, "\033[0m");
	va_end(args);

	return;
}

static PORTBIT_STAT prettyPrintBoard(struct bitfield *field)
{
	short int square_color = 0;
	size_t atlas_i, piece_i;

	if (field == NULL)
	{
		return PORTBIT_FAILURE;
	}

	fputc('\n', stdout);

	for (atlas_i = 0, piece_i = BOARD_ATLAS_BIT_LEN; 
		atlas_i < BOARD_ATLAS_BIT_LEN; atlas_i++)
	{
		unsigned char has_piece = 0;

		readBitsIntoByte(field, 1, atlas_i, &has_piece);

		if ((atlas_i > 0) && (atlas_i % 8 == 0))
		{
			square_color++;
			fputc('\n', stdout);
		}

		if (has_piece != 0)
		{
			unsigned char raw_piece = 0;

			if (readBitsIntoByte(field, 4, piece_i, &raw_piece) 
				== PORTBIT_SUCCESS)
			{
				const PORTBIT_BOOL is_white 
					= ((raw_piece & 1 << 3) != 0);
				const char out_piece = decodePiece(raw_piece);

				clrPrintf((is_white == PORTBIT_TRUE) 
						? ANSI_BRIGHT_WHITE 
						: ANSI_BLACK, 
					(square_color % 2) 
						? ANSI_BRIGHT_BLUE
						: ANSI_BRIGHT_RED,
					"%c", out_piece);

				piece_i += 4;
			}
		}
		else
		{
			clrPrintf(ANSI_WHITE, (square_color % 2)
				? ANSI_BRIGHT_BLUE : ANSI_BRIGHT_RED, " ");
		}

		square_color++;
	}

	fputc('\n', stdout);

	return PORTBIT_SUCCESS;
}

/* You should be able to look at the header, or board state equivelent, and 
 * calculate the size of the bitfield you will need to serialize it immediately
 * the nice thing is that this board state will never exceed 24 bytes, unless
 * you decide that you need extra information */
int main(int argc, char **argv)
{
	struct bitfield *field = newBitfield(CHESS_STATE_BIT_LEN);

	(void) argc;
	(void) argv;

	if (field == NULL)
	{
		return PORTBIT_FAILURE;
	}

	if (serializeBoard(kasp_immortal, field) == PORTBIT_SUCCESS)
	{
		dumpBitfield(field);
		prettyPrintBoard(field);
	}

	freeBitfield(field);

	return PORTBIT_SUCCESS;
}

