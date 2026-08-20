#define _GNU_SOURCE


#include <err.h>
#include <errno.h>
#include <error.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main () {
	FILE *f = fopen ("./nofile", "r");	

//	string.h
//	char * strerror (int errnum);
//	printf ("errno的意思是：%s\n", strerror (errno));
//	换个区域
//	setlocale (LC_ALL, "zh_CN.utf8");
//	printf ("errno的意思是：%s\n", strerror (errno));

//	string.h
//	char * strerror_l (int errnum, locale_t locale);
//	locale_t loc = newlocale (LC_ALL_MASK, "zh_CN.utf8", 0);
//	printf ("errno的意思是：%s\n", strerror_l (errno, loc));

//	string.h
//	char * strerror_r (int errnum, char *buf, size_t n);
//	GNU版本，需要定义宏_GNU_SOURCE
//	int buf_size = 100;
//	char *buf = malloc (buf_size);
//	char *res = strerror_r (errno, buf, buf_size);
//	printf ("buf是：%s\n", buf);
//	printf ("res是：%s\n", res);

//	string.h
//	int strerror_r (int errnum, char *buf, size_t n);
//	POSIX版本，编译器默认使用这个
//	int buf_size = 100;
//	char *buf = malloc (buf_size);
//	int res = strerror_r (errno, buf, buf_size);
//	printf ("buf是：%s\n", buf);
//	printf ("res是：%d\n", res);

//	stdio.h
//	void perror (const char *message);
//	perror ("");

//	string.h
//	const char * strerrorname_np (int errnum)
//	需要定义宏_GNU_SOURCE
//	printf ("errno的名字是：%s\n", strerrorname_np (errno));

//	string.h
//	const char * strerrordesc_np (int errnum)
//	需要定义宏_GNU_SOURCE
//	printf ("errno的意思是：%s\n", strerrordesc_np (errno));

//	errno.h
//	char * program_invocation_name
//	需要定义宏_GNU_SOURCE
//	printf ("当前的程序是这样调用的：%s\n", program_invocation_name);

//	errno.h
//	char * program_invocation_short_name
//	需要定义宏_GNU_SOURCE
//	printf ("当前的程序是：%s\n", program_invocation_short_name);

//	error.h，注意这是error.h，不要搞错了！
//	void error (int status, int errnum, const char *format, …)
//	void my_error_print ();
//	error_print_progname = my_error_print;
//	error (0, errno, "格式字符串");

//	error.h
//	void error_at_line (int status, int errnum, const char *fname, unsigned int lineno, const char *format, …)
//	printf ("现有%d条错误信息\n", error_message_count);
//	error_at_line (0, errno, __FILE__, __LINE__, "格式字符串");
//	printf ("现有%d条错误信息\n", error_message_count);

//	error.h
//	void (*error_print_progname) (void)
//	上文已使用

//	error.h
//	unsigned int error_message_count
//	上文已使用

//	error.h
//	int error_one_per_line
//	error_one_per_line = 1;
//	error_at_line (0, errno, __FILE__, 13, "格式字符串");
//	error_at_line (0, errno, __FILE__, 13, "格式字符串");

//	err.h
//	void warn (const char *format, …)
//	printf ("现有%d条错误信息\n", error_message_count);
//	warn ("这是warn的输出");
//	printf ("现有%d条错误信息\n", error_message_count);

//	err.h
//	void vwarn (const char *format, va_list ap)
//	略

//	err.h
//	void warnx (const char *format, …)
//	warnx ("这是warnx的输出");

//	err.h
//	void vwarnx (const char *format, va_list ap)
//	略

//	err.h
//	void err (int status, const char *format, …)
//	printf ("现有%d条错误信息\n", error_message_count);
//	err (0, "这是err的输出");
//	printf ("现有%d条错误信息\n", error_message_count);

//	err.h
//	void verr (int status, const char *format, va_list ap)
//	略

//	err.h
//	void errx (int status, const char *format, …)
//	errx (0, "这是errx的输出");

//	err.h
//	void verrx (int status, const char *format, va_list ap)
//	略

	return 0;
}

//	void my_error_print () {
//		fprintf (stderr, "我的错误: ");
//	}
