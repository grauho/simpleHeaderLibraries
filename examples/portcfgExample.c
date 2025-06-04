#include <stdio.h>
#include <stdlib.h>

#include "../portcfg.h"

static int configCallback(const char * const head, const char * const key,
	const char * const value, void *data)
{
	(void) data;

	fprintf(stdout, "[%s] %s = %s\n", 
		(head[0] != '\0') ? head : "GLOBAL", key, value);

	return 0;
}

int main(int argc, char **argv)
{
	FILE *file_handle = NULL;
	char *example_cfg = "./exampleConfig.cfg";
	int i;

	if (argc < 2)
	{
		if ((file_handle = fopen("./exampleConfig.cfg", "rb")) == NULL)
		{
			fprintf(stderr, "Failed to load file from '%s'\n", 
				example_cfg);
		}

		portcfgProcess(file_handle, configCallback, NULL);
	}
	else
	{
		for (i = 1; i < argc; i++)
		{
			if ((file_handle = fopen(argv[i], "rb")) == NULL)
			{
				fprintf(stderr, 
					"Failed to load file from '%s'\n",
					argv[i]);

				continue;
			}

			portcfgProcess(file_handle, configCallback, NULL);
		}
	}

	return 0;
}

