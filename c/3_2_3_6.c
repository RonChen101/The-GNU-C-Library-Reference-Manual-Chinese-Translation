#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


int
main ()
{
//	stdlib.h
//	void * calloc (size_t count, size_t eltsize)
	int * a = calloc(3, sizeof (int));
	printf ("calloc成功！\terrno = %d\n", errno);

	return 0;
}
