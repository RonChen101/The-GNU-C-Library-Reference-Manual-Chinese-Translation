#include <stdio.h>
#include <stdlib.h>


int
main ()
{
//	stdlib.h
//	void free (void *ptr)
	int * a = malloc (2 * sizeof (int));
	free (a);

//	stdlib.h
//	void free_sized (void *ptr, size_t size)
	size_t a_size = 3 * sizeof (char);
	a = malloc (a_size);
	free_sized (a, a_size); 

	return 0;
}
