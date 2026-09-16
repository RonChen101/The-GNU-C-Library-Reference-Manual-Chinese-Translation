#include <errno.h>
#include <stdio.h>
#include <obstack.h>

#define obstack_chunk_alloc	xmalloc
#define obstack_chunk_free	free

int
main ()
{
	struct obstack * my_obstack;

//	obstack.h
//	int obstack_init (struct obstack *obstack-ptr)
	obstack_init (my_obstack);

//	obstack.h
//	obstack_alloc_failed_handler
//	略

	return 0;
}
