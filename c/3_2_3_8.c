#include <errno.h>
#include <malloc.h>
#include <stdio.h>

int
main ()
{
//	malloc.h
//	int mallopt (int param, int value)
	mallopt (M_MMAP_MAX, 123);
	printf ("mallopt成功！\terrno = %d\n", errno);

	return 0;
}
