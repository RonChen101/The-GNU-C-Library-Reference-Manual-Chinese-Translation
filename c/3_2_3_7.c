#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int
main ()
{
//	stdlib.h
//	void * aligned_alloc (size_t alignment, size_t size)
//	由下面的程序可知，aligned_alloc只会使a对齐，即a[0]的地址为8的倍数，但是数组中其他项的地址就不一定是8的倍数了！
	int * a = aligned_alloc (8, 3 * sizeof (int));
	if (errno == 0)
	{
		printf ("aligned_alloc成功！\terrno = %d\n", errno);
		for (int i = 0; i < 3; i++)
		{
			printf ("a[%d]的地址为：\t%p\n", i, & a[i]);
		}
	}

//	malloc.h
//	void * memalign (size_t boundary, size_t size)
	int * b = memalign (8, 3 * sizeof (int));
	printf ("memalign成功！\terrno = %d\n", errno);

//	stdlib.h
//	int posix_memalign (void **memptr, size_t alignment, size_t size)
	printf ("sizeof (void *) = %zu\n", sizeof (void *));
	int * c;
	posix_memalign ((void **) (& c), 2 * sizeof (void *), 2 * sizeof (int));
	printf ("posix_memalign成功！\terrno = %d\n", errno);

//	malloc.h或stdlib.h
//	void * valloc (size_t size)
	printf ("getpagesize = %d字节\n", getpagesize ());
	int * d = valloc (3 * sizeof (int));
	printf ("valloc成功！\terrno = %d\n", errno);

//	stdlib.h
//	size_t memalignment (void *p)
//	它只是通过计算p的二进制末尾有多少个0来得到该地址的最大对齐度。如果地址刚好落在更大粒度的边界上（例如Cache Line或Page边界），它就会返回更大的对齐值。
	size_t align = memalignment ((void *) a);
	printf ("a的对齐方式为\t%zu\n", align);
	align = memalignment ((void *) b);
	printf ("b的对齐方式为\t%zu\n", align);
	align = memalignment ((void *) c);
	printf ("c的对齐方式为\t%zu\n", align);
	align = memalignment ((void *) d);
	printf ("d的对齐方式为\t%zu\n", align);

	return 0;
}
