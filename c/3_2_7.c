#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int
main ()
{
//	stdlib.h
//	void * alloca (size_t size)
	int * a = alloca (sizeof (int));
	printf ("alloca成功！\terrno = %d\n", errno);

	return 0;
}
