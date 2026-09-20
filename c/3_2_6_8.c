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
//	void * obstack_base (struct obstack *obstack-ptr)
	printf ("obstack_base\t\t= %p\n", obstack_base (my_obstack));
	printf ("obstack_next_free\t= %p\n", obstack_next_free (my_obstack));

	obstack_1grow_fast (my_obstack, 'z');
	printf ("\n调用了obstack_1grow_fast\n\n");

	printf ("obstack_base\t\t= %p\n", obstack_base (my_obstack));
	printf ("obstack_next_free\t= %p\n", obstack_next_free (my_obstack));

	char * a = obstack_finish (my_obstack);
	printf ("\n调用了obstack_finish\n\n");

	printf ("obstack_base\t\t= %p\n", obstack_base (my_obstack));
	printf ("obstack_next_free\t= %p\n", obstack_next_free (my_obstack));

//	obstack.h
//	void * obstack_next_free (struct obstack *obstack-ptr)
//	略

//	obstack.h
//	int obstack_object_size (struct obstack *obstack-ptr)
//	略

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
