#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <obstack.h>

#define obstack_chunk_alloc	xmalloc
#define obstack_chunk_free	free

void *
xmalloc (size_t size);

int
main ()
{
	struct obstack * my_obstack = malloc (sizeof (struct obstack));
	obstack_init (my_obstack);

//	obstack.h
//	void * obstack_alloc (struct obstack *obstack-ptr, int size)
	int * a = obstack_alloc (my_obstack, 3 * sizeof (int));

//	obstack.h
//	void * obstack_copy (struct obstack *obstack-ptr, void *address, int size)
//	这里的size可能要写n + 1？
	char * b = obstack_copy (my_obstack, "你好", 7 * sizeof (char));
	printf ("%s\t%zu\n", b, strlen (b));

//	obstack.h
//	void * obstack_copy0 (struct obstack *obstack-ptr, void *address, int size)
	char * c = obstack_copy0 (my_obstack, "我也好", 9 * sizeof (char));
	printf ("%s\t%zu\n", c, strlen (c));

	return 0;
}

void *
xmalloc (size_t size)
{
	void *p = malloc (size);
	if (p == NULL)
//		fatal ("virtual memory exhausted");
		perror ("virtual memory exhausted");
	return p;
}
