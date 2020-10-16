#include <string.h>
#include "header.h"

int segPlano (int argc, char* argv[]) 
{
	if (!strcmp (argv[argc-1], "&"))  //Si el último parámetro es &, lo cambio por NULL y devuevo 1
	{	
		argv[argc-1] = NULL;
		return 1;
	}
	return 0;
}
