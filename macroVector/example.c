/* A tiny slice of what cevMacro can do, refer to the manual page for more
 * information on the more advanced capabilities of the library */

#include <stdio.h>
#include <stdlib.h>

#include "cevMacro.h"

struct exampleDataType
{
	int id;
	char *str;
};

CEV_MACRO_COMPLETE(example, struct exampleDataType);

int main(int argc, char **argv)
{
	struct exampleVector *vec = exampleVectorInit(10);
	struct exampleDataType tmp;
	int i;

	for (i = 1; i < argc; i++)
	{
		tmp.id = i;
		tmp.str = argv[i];

		if (i % 2)
		{
			exampleVectorPush(vec, tmp);
		}
		else
		{
			exampleVectorPushBack(vec, tmp);
		}
	}

	while (exampleVectorPop(vec, &tmp) == CEV_SUCCESS)
	{
		fprintf(stdout, "%i: %s\n", tmp.id, tmp.str);
	}

	exampleVectorFree(vec);

	return 0;
}

