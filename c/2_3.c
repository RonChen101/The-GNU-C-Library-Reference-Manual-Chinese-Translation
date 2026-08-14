//	#define _GNU_SOURCE


#include<errno.h>
#include<locale.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int main() {
	FILE *f = fopen("./nofile", "r");

//	string.h
//	char * strerror (int errnum);
//	printf("errno的意思是：%s\n", strerror(errno));
	//	换个区域
//	setlocale(LC_ALL, "zh_CN.utf8");
//	printf("errno的意思是：%s\n", strerror(errno));

//	string.h
//	char * strerror_l (int errnum ,locale_t locale);
//	locale_t loc = newlocale(LC_ALL_MASK, "zh_CN.utf8", 0);
//	printf("errno的意思是：%s\n", strerror_l(errno, loc));

//	string.h
//	char * strerror_r (int errnum, char *buf, size_t n);
//	GNU版本，需要定义宏_GNU_SOURCE
//	int buf_size = 100;
//	char *buf = malloc(buf_size);
//	char *res = strerror_r(errno, buf, buf_size);
//	printf("buf是：%s\n", buf);
//	printf("res是：%s\n", res);

//	string.h
//	int strerror_r (int errnum, char *buf, size_t n);
//	POSIX版本，编译器默认使用这个
//	int buf_size = 100;
//	char *buf = malloc(buf_size);
//	int res = strerror_r(errno, buf, buf_size);
//	printf("buf是：%s\n", buf);
//	printf("res是：%d\n", res);

//	stdio.h
//	void perror (const char *message);
//	perror("");



	return 0;
}
