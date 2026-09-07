#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


int
main ()
{
//	stdlib.h
//	void * realloc (void *ptr, size_t newsize)
	int * a = malloc (sizeof (int));
	int * temp = realloc (a, 2 * sizeof (int));
	if (temp != NULL)
	{
		a = temp;
		printf ("realloc成功！\t\terrno = %d\n", errno);
	}

//	stdlib.h
//	void * reallocarray (void *ptr, size_t nmemb, size_t size)
	temp = reallocarray (a, 3, sizeof (int));
	if (temp != NULL)
	{
		a = temp;
		printf ("reallocarray成功！\terrno = %d\n", errno);
	}

	return 0;
}
