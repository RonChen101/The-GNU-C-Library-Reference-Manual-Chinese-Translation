#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


int
main ()
{
//	stdlib.h
//	void free (void *ptr)
	int * a = malloc (2 * sizeof (int));
	free (a);
	printf ("free成功！\t\t\terrno = %d\n", errno);

//	stdlib.h
//	void free_sized (void *ptr, size_t size)
//	glibc版本至少为2.43，glibc版本可以通过ldd --version查看，Ubuntu 26自带glibc 2.43！
	size_t b_size	= 3 * sizeof (char);
	char * b		= malloc (b_size);
	free_sized (b, b_size); 
	printf ("free_sized成功！\t\terrno = %d\n", errno);

//	stdlib.h
//	void free_aligned_sized (void *ptr, size_t alignment, size_t size)
//	glibc版本至少为2.43，glibc版本可以通过ldd --version查看，Ubuntu 26自带glibc 2.43！
	size_t c_alignment	= 16;
	size_t c_size		= 4 * sizeof (float);
	float * c			= aligned_alloc (c_alignment, c_size);
	free_aligned_sized (c, c_alignment, c_size);
	printf ("free_aligned_sized成功！\terrno = %d\n", errno);

	return 0;
}
