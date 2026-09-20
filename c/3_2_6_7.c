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
//	int obstack_room (struct obstack *obstack-ptr)
	printf ("room\t= %d\n", obstack_room (my_obstack));

//	obstack.h
//	void obstack_1grow_fast (struct obstack *obstack-ptr, char c)
	obstack_1grow_fast (my_obstack, 'z');
	char * a = obstack_finish (my_obstack);
	printf ("a\t= %c\t%p\n", * a, a);

//	obstack.h
//	void obstack_ptr_grow_fast (struct obstack *obstack-ptr, void *data)
	float b = 2.71;
	obstack_ptr_grow_fast (my_obstack, & b);
	float ** c = obstack_finish (my_obstack);
	printf ("c\t= %.2f\t%p\t%p\n", ** c, c, * c);

//	obstack.h
//	void obstack_int_grow_fast (struct obstack *obstack-ptr, int data)
	obstack_int_grow_fast (my_obstack, 11);
	int * d = obstack_finish (my_obstack);
	printf ("d\t= %d\t%p\n", * d, d);

//	obstack.h
//	void obstack_blank_fast (struct obstack *obstack-ptr, int size)
	obstack_blank_fast (my_obstack, 2 * sizeof (short));
	short * e = obstack_finish (my_obstack);
	e [0] = 1;
	e [1] = 2;
	printf ("e [0]\t= %d\t%p\n", e [0], & e [0]);
	printf ("e [1]\t= %d\t%p\n", e [1], & e [1]);
	printf ("room\t= %d\n", obstack_room (my_obstack));

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
