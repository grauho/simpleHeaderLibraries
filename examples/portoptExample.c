#include <stdio.h>
#include <stdlib.h>

#include "../portopt.h"

static void printHelp(void)
{
	fputs("portopt.h demo text pass-through program\n\n", stderr);
	fputs("Command Line Arguments:\n", stderr);
	fputs("\t-v, --verbose  : Enables extra logging output\n", stderr);
	fputs("\t-h, --help     : Prints this message then exits\n", stderr);
	fputs("\t-o, --output   : Output path, default stdout\n", stderr);
	fputs("\t-i, --input    : Input path, default stdin\n", stderr);
	fputs("\nUsage\n", stderr);
	fputs("\t./a.out -i foo.txt -o bar.txt -v\n\n", stderr);
	fputs("Notes:\n", stderr);
	fputs("\tIf using stdin for input CTRL-D will terminate program\n\n", 
		stderr);
}

int main(int argc, char **argv)
{
	const struct portoptVerboseOpt opts[] =
	{
		{'v', "verbose", PORTOPT_FALSE},
		{'h', "help",    PORTOPT_FALSE},
		{'o', "output",  PORTOPT_TRUE},
		{'i', "input",   PORTOPT_TRUE}
	};
	const size_t num_opts = sizeof(opts) / sizeof(opts[0]);
	const size_t argl = (size_t) argc;
	size_t ind = 0;
	int flag;

	FILE *output, *input;
	char *out_path = NULL;
	char *in_path = NULL;
	PORTOPT_BOOL verbose = PORTOPT_FALSE;

	int ch;

	while ((flag = portoptVerbose(argl, argv, opts, num_opts, &ind)) != -1)
	{
		switch (flag)
		{
			case 'v':
				verbose = PORTOPT_TRUE;
				break;
			case 'h':
				printHelp();
				return 0;
			case 'o':
				out_path = portoptGetArg(argl, argv, &ind);
				break;
			case 'i':
				in_path = portoptGetArg(argl, argv, &ind);
				break;
			case '?': /* fallthrough */
			default:
				fprintf(stderr, "Unknown option %c\n", flag);
				break;
		}
	}

	/* After parsing ind will point to the last argument consumed, the
	 * user may make use of this to process futher command line args. 
	 * This functionality is not needed for this example program.
	 * eg: if one wanted to also skip the program name:
	 * i = ind += (ind == 0) */

	if (in_path == NULL)
	{
		if (verbose == PORTOPT_TRUE)
		{
			fputs("Using stdin for program input\n", stderr);
			fputs("Use CTRL-D to send EOF and exit program\n",
				stderr);
		}

		input = stdin;
	}
	else if ((input = fopen(in_path, "rb")) == NULL)
	{
		abort();
	}

	if (out_path == NULL)
	{
		if (verbose == PORTOPT_TRUE)
		{
			fputs("Using stdout for program output\n", stderr);
		}

		output = stdout;
	}
	else if ((output = fopen(out_path, "wb")) == NULL)
	{
		abort();
	}

	while ((ch = fgetc(input)) != EOF)
	{
		fputc(ch, output);
	}

	if (in_path != NULL)
	{
		fclose(input);
	}

	if (out_path != NULL)
	{
		fclose(output);
	}

	return 0;
}

