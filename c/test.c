#include <stdio.h>
#include <stdlib.h>


int
main ()
{
	char * a	= malloc (2 * sizeof (char));
	a[0]		= 1;
	printf ("a\t\t= %p\n",			a);
	for (int i = -16; i <= 16; i++)
	{
		if (i < 0)
		{
			printf ("* (a - %d)\t= %d\n", -i, * (a + i));
		}
		else if (i == 0)
		{
			printf ("* a\t\t= %d\n", * a);
		}
		else
		{
			printf ("* (a + %d)\t= %d\n", i, * (a + i));
		}
	}

	short * b	= malloc (sizeof (short));
	b[0]		= 1;
	printf ("b\t\t= %p\n",			b);
	for (int i = -16; i <= 16; i++)
	{
		if (i < 0)
		{
			printf ("* (b - %d)\t= %d\n", -i, * (b + i));
		}
		else if (i == 0)
		{
			printf ("* b\t\t= %d\n", * b);
		}
		else
		{
			printf ("* (b + %d)\t= %d\n", i, * (b + i));
		}
	}


	int * c	= malloc (sizeof (int));
	c[0]	= 1;
	printf ("c\t\t= %p\n",			c);
	for (int i = -16; i <= 16; i++)
	{
		if (i < 0)
		{
			printf ("* (c - %d)\t= %d\n", -i, * (c + i));
		}
		else if (i == 0)
		{
			printf ("* c\t\t= %d\n", * c);
		}
		else
		{
			printf ("* (c + %d)\t= %d\n", i, * (c + i));
		}
	}

	return 0;
}
