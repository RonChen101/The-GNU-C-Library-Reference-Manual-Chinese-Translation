#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int
main ()
{
//	unistd.h
//	int brk (void *addr)
//	略

//	unistd.h
//	void * sbrk (ptrdiff_t delta)
	printf ("sbrk (0) = %p\n", sbrk (0));

	return 0;
}
