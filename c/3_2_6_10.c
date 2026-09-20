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
//	int obstack_chunk_size (struct obstack *obstack-ptr)
//	不知道为什么编译器提示我用%ld，笑哭
	printf ("obstack_chunk_size = %ld\n", obstack_chunk_size (my_obstack));

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
