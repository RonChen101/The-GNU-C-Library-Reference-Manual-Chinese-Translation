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
//	void obstack_blank (struct obstack *obstack-ptr, int size)
	obstack_blank (my_obstack, 3 * sizeof (int));
	int * a = obstack_finish (my_obstack);
	a [0] = 1;
	a [1] = 2;
	a [2] = 3;
	for (int i = 0; i < 3; i++)
	{
		printf ("a [%d]\t\t= %d\t%p\n", i, a [i], & a [i]);
	}

//	obstack.h
//	void obstack_grow (struct obstack *obstack-ptr, void *data, int size)
	obstack_grow (my_obstack, "可乐", 7 * sizeof (char));
	char * b = obstack_finish (my_obstack);
	printf ("b\t\t= %s\t%p\n", b, b);

//	obstack.h
//	void obstack_grow0 (struct obstack *obstack-ptr, void *data, int size)
	obstack_grow0 (my_obstack, "牛福", 6 * sizeof (char));
	char * c = obstack_finish (my_obstack);
	printf ("c\t\t= %s\t%p\n", c, c);

//	obstack.h
//	void obstack_1grow (struct obstack *obstack-ptr, char c)
	obstack_1grow (my_obstack, 'l');
	char * d = obstack_finish (my_obstack);
	printf ("d\t\t= %s\t%p\n", d, d);

//	obstack.h
//	void obstack_ptr_grow (struct obstack *obstack-ptr, void *data)
	float e = 3.14;
	obstack_ptr_grow (my_obstack, &e);
	float ** f = obstack_finish (my_obstack);
	printf ("f\t\t= %.2f\t%p\t%p\n", ** f, f, *f);

//	obstack.h
//	void obstack_int_grow (struct obstack *obstack-ptr, int data)
	obstack_int_grow (my_obstack, 67);
	printf ("object_size\t= %d\n", obstack_object_size (my_obstack));
	int * g = obstack_finish (my_obstack);
	printf ("g\t\t= %d\t%p\n", * g, g);
	printf ("object_size\t= %d\n", obstack_object_size (my_obstack));

//	obstack.h
//	void * obstack_finish (struct obstack *obstack-ptr)
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
