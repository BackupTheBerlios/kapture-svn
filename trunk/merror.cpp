#include <stdio.h>

void KError(char* error, int errorno)
{
#ifdef DEBUG
	printf(" E %s (Error %d)\n", error, errorno);
#endif
}
