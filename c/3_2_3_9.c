#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <mcheck.h>

void
print_mcheck_status (enum mcheck_status status);

int
main ()
{
//	mcheck.h
//	int mcheck (void (*abortfn) (enum mcheck_status status))
//	想要使用这个函数，需要在编译好的可执行文件，比如a.out，前面加上LD_PRELOAD的配置，以链接libc_malloc_debug.so文件，下面是参考命令：
//		gcc 源文件.c && LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libc_malloc_debug.so ./a.out
	mcheck (print_mcheck_status);
	printf ("mcheck成功！\terrno = %d\n", errno);

	int * a = malloc (3 * sizeof (int));

	a[-2]	= 123;
//	a[-1]	= 1;
	printf ("a[-2]\t= %d\n", a[-2]);

//	a[3]	= 1;
	a[4]	= 456;
	printf ("a[4]\t= %d\n", a[4]);

	free (a);

//	mcheck.h
//	enum mcheck_status mprobe (void *pointer)
//	注意，第一，调用mprobe一定要在程序开头调用mcheck；第二，调用mprobe后，也有概率触发mcheck的回调函数。
	int * b = malloc (2 * sizeof (int));

	enum mcheck_status b_status = mprobe (b);
	printf ("mprobe成功！\terrno = %d\n", errno);
	print_mcheck_status (b_status);

	for (int i = 0; i < 10; i++)
	{
		b[i] = 1;
	}
	b_status = mprobe (b);
	printf ("mprobe成功！\terrno = %d\n", errno);
	print_mcheck_status (b_status);

//	mcheck.h
//	enum mcheck_status
//	略

	return 0;
}

void
print_mcheck_status (enum mcheck_status status)
{
	switch (status)
	{
	case MCHECK_DISABLED:
		printf ("mcheck was not called before the first allocation. No consistency checking can be done.\n");
		break;
	case MCHECK_OK:
//		mcheck不会在一切正常时，调用回调函数。
		printf ("No inconsistency detected.\n");
		break;
	case MCHECK_HEAD:
		printf ("The data immediately before the block was modified. This commonly happens when an array index or pointer is decremented too far.\n");
		break;
	case MCHECK_TAIL:
		printf ("The data immediately after the block was modified. This commonly happens when an array index or pointer is incremented too far.\n");
		break;
	case MCHECK_FREE:
//		mcheck函数应该也不会输出这里的内容。
		printf ("The block was already freed.\n");
		break;
	}
}
