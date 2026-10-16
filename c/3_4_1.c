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
	setlocale (LC_ALL, "zh_CN.utf8");

//	sys/mman.h
//	PKEY_DISABLE_READ, PKEY_DISABLE_WRITE, PKEY_DISABLE_ACCESS, PKEY_DISABLE_EXECUTE
//	这些都是宏，但是我的glibc库好像只定义了PKEY_DISABLE_ACCESS和PKEY_DISABLE_WRITE，分别是1和2

//	sys/mman.h
//	int pkey_alloc (unsigned int flags, unsigned int access_restrictions)
//	需要定义宏_GNU_SOURCE，AI说，lscpu的输出结果中Flags中没有pku的话，pkey_alloc函数就用不了。
//	以及https://man7.org/linux/man-pages/man7/pkeys.7.html中的这段文字：
//	Although unnecessary, hardware support for protection keys may be enumerated with the cpuid instruction. Details of how to do this can be found in the Intel Software Developers Manual. The kernel performs this enumeration and exposes the information in /proc/cpuinfo under the "flags" field. The string "pku" in this field indicates hardware support for protection keys and the string "ospke" indicates that the kernel contains and has enabled protection keys support.
//	随便提一嘴，阿里云的g8i规格族的服务器支持pku。
	int protect_key = pkey_alloc (0, PKEY_DISABLE_WRITE);
	if (protect_key != -1)
	{
		printf ("pkey_alloc成功！\n\n");
	}
	else
	{
		printf ("pkey_alloc失败！\terrno = %d\n", errno);
		printf ("errno的意思是：%s\n\n", strerror (errno));
	}

//	sys/mman.h
//	int pkey_free (int key)
	int result = pkey_free (protect_key);
	if (result != -1)
	{
		printf ("pkey_free成功！\n\n");
	}
	else
	{
		printf ("pkey_free失败！\terrno = %d\n", errno);
		printf ("errno的意思是：%s\n\n", strerror (errno));
	}

//	sys/mman.h
//	int pkey_mprotect (void *address, size_t length, int protection, int key)
	size_t pagesize = sysconf (_SC_PAGESIZE);
	void * addr = mmap (
		NULL,
		pagesize,
		PROT_READ,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1,
		0
	);
	protect_key = pkey_alloc (0, PKEY_DISABLE_WRITE);
	result = pkey_mprotect (addr, pagesize, PROT_READ | PROT_WRITE, protect_key);	
	if (result != -1)
	{
		printf ("pkey_mprotect成功！\n\n");
	}
	else
	{
		printf ("pkey_mprotect失败！\terrno = %d\n", errno);
		printf ("errno的意思是：%s\n\n", strerror (errno));
	}

//	sys/mman.h
//	int pkey_set (int key, unsigned int access_restrictions)
	protect_key = pkey_alloc (0, PKEY_DISABLE_WRITE);
	result = pkey_set (protect_key, PKEY_DISABLE_ACCESS);
	if (result != -1)
	{
		printf ("pkey_set成功！\n\n");
	}
	else
	{
		printf ("pkey_set失败！\terrno = %d\n", errno);
		printf ("errno的意思是：%s\n\n", strerror (errno));
	}

//	sys/mman.h
//	int pkey_get (int key)
	result = pkey_get (protect_key);
	if (result != -1)
	{
		printf ("pkey_get成功！\n");
		if (result & PKEY_DISABLE_ACCESS)
		{
			printf ("含有PKEY_DISABLE_ACCESS！\n");
		}
		if (result & PKEY_DISABLE_WRITE)
		{
			printf ("含有PKEY_DISABLE_WRITE！\n");
		}
	}
	else
	{
		printf ("pkey_get失败！\terrno = %d\n", errno);
		printf ("errno的意思是：%s\n\n", strerror (errno));
	}

	return 0;
}
