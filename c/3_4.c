#define _GNU_SOURCE

#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int
main ()
{
//	sys/mman.h
//	PROT_WRITE, PROT_READ, PROT_EXEC, PROT_NONE
//	这些都是宏，分别是1，2，4，0

//	sys/mman.h
//	int mprotect (void *address, size_t length, int protection)
	size_t pagesize = sysconf (_SC_PAGESIZE);
	void * addr = mmap (
		NULL,
		pagesize,
		PROT_READ,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1,
		0
	);
	int result = mprotect (addr, pagesize, PROT_READ | PROT_WRITE);
	if (result == 0)
	{
		printf ("mprotect成功！\n");
	}
	else
	{
		printf ("mprotect失败！\terrno = %d\n", errno);
	}

//	sys/mman.h
//	int mseal (void *address, size_t length, unsigned long flags)
//	需要定义宏_GNU_SOURCE
	result = mseal (addr, pagesize, 0);
	setlocale (LC_ALL, "zh_CN.utf8");
//	Ubuntu上，locale -a命令的输出如果没有zh_CN.utf8，可以通过这条命令解决：
//		apt install language-pack-zh-hans
	if (result == 0)
	{
		printf ("mseal成功！\n");
	}
	else
	{
		printf ("mseal失败！\terrno = %d\n", errno);
		printf ("errno的意思是：%s\n", strerror (errno));
	}
	result = mprotect (addr, pagesize, PROT_EXEC);
	if (result == 0)
	{
		printf ("mprotect成功！\n");
	}
	else
	{
		printf ("mprotect失败！\terrno = %d\n", errno);
		printf ("errno的意思是：%s\n", strerror (errno));
	}
	result = munmap (addr, pagesize);
	if (result == 0)
	{
		printf ("munmap成功！\n");
	}
	else
	{
		printf ("munmap失败！\terrno = %d\n", errno);
		printf ("errno的意思是：%s\n", strerror (errno));
	}

	return 0;
}
