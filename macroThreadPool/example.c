#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "macroThreadPool.h"

struct myArgs
{
	int id;
	char *str;
};

static void workerFunction(struct myArgs args)
{
	usleep(rand() % 5);
	fprintf(stdout, "%d, %s\n", args.id, args.str);
}

/* this could also take a myArgs pointer instead of a value copy but since
 * nothing is being passed out of the function into the struct this is easier
 * as it requires no additional allocation */
MACRO_THREAD_POOL_COMPLETE(foo, struct myArgs, workerFunction);

static void printHelp(void)
{
	fputs("Simple Macro Thread Pool Example Program:\n\n", stdout);
	fputs("Usage:\n", stdout);
	fputs("\t./mtpExample [args]...\n", stdout);
	fputs("\n", stdout);
	fputs("Must supply a non-zero number of args, ideally multiple\n\n",
		stdout);
}

int main(int argc, char **argv)
{
	struct fooThreadPool *pool = fooNewThreadPool(7, 7);	
	unsigned int seed = 0;
	size_t i;

	if (argc < 2)
	{
		printHelp();

		return 1;
	}

	/* Lazy way to initialize random number generator */
	for (i = 0; argv[1][i] != '\0'; i++)
	{
		seed += argv[1][i];
	}

	srand(seed);

	if (pool == NULL)
	{
		fprintf(stderr, "Failed to allocate thread pool\n");

		return 1;
	}

	for (i = 1; i < argc; i++)
	{
		struct myArgs args = {0};

		args.id = i;
		args.str = argv[i];
		fooEnqueueJob(pool, args);
	}

	fooCleanupThreadPool(pool);

	return 0;
}

