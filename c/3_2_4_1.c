#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <mcheck.h>

int
main ()
{
//	mcheck.h
//	void mtrace (void)
//  想要使用这个函数，第一，需要定义环境变量MALLOC_TRACE；第二，需要在编译好的可执行文件，比如a.out，前面加上LD_PRELOAD的配置，以链接libc_malloc_debug.so文件。下面是参考命令：
//      export MALLOC_TRACE=/root/glibc_man/c/mtrace.log && gcc 源文件.c && LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libc_malloc_debug.so ./a.out
	mtrace ();
	printf ("mcheck成功！\terrno = %d\n", errno);

	int * a = malloc (3 * sizeof (int));
	printf ("a =\t%p\n", a);
	free (a);
/*
上面的代码运行完成后，mtrace.log中多出来的内容，第三行和第四行的地址应该和上面的代码打印出来的相同。
  1 = Start
  2 @ /usr/lib/x86_64-linux-gnu/libc.so.6:(_IO_file_doallocate+8c)[0x8ba0c] + 0x635b10fc0510 0x400
  3 @ ./a.out:[0x1201] + 0x635b10fc0010 0xc
  4 @ ./a.out:[0x1211] - 0x635b10fc0010
  5 @ /usr/lib/x86_64-linux-gnu/libc.so.6:[0x9e34c] - 0x635b10fc0510
*/

//	mcheck.h
//	void muntrace (void)
//	略

	return 0;
}
