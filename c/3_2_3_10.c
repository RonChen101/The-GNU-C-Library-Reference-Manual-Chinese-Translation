#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

int
main ()
{
//	malloc.h
//	struct mallinfo2
//	略

//	malloc.h
//	struct mallinfo2 mallinfo2 (void)
	int * a = malloc (3 * sizeof (int));
	struct mallinfo2 mallinfo = mallinfo2 ();
	printf ("arena =\t\t%zu\n",		mallinfo . arena);
	printf ("ordblks =\t%zu\n",		mallinfo . ordblks);
	printf ("smblks =\t%zu\n",		mallinfo . smblks);
	printf ("hblks =\t\t%zu\n",		mallinfo . hblks);
	printf ("hblkhd =\t%zu\n",		mallinfo . hblkhd);
	printf ("usmblks =\t%zu\n",		mallinfo . usmblks);
	printf ("fsmblks =\t%zu\n",		mallinfo . fsmblks);
	printf ("uordblks =\t%zu\n",	mallinfo . uordblks);
	printf ("fordblks =\t%zu\n",	mallinfo . fordblks);
	printf ("keepcost =\t%zu\n",	mallinfo . keepcost);

	return 0;
}
