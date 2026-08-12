//	#define _POSIX_C_SOURCE 200809L
//	#define _GNU_SOURCE


#include<errno.h>
#include<locale.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


//	locale.h
char * setlocale (int category, const char *locale);
locale_t newlocale(int category_mask, const char *locale, locale_t base);


//	stdio.h
void perror (const char *message);


//	string.h
char * strerror (int errnum);
char * strerror_l (int errnum ,locale_t locale);
//	char * strerror_r (int errnum, char *buf, size_t n);
int strerror_r (int errnum, char *buf, size_t n);


int main() {
	FILE *f = fopen("./nofile", "r");

//	printf("errno的意思是：%s\n", strerror(errno));

//	perror("");

//	setlocale(LC_ALL, "zh_CN.utf8");
//	printf("errno的意思是：%s\n", strerror(errno));

//	locale_t loc = newlocale(LC_ALL_MASK, "zh_CN.utf8", 0);
//	printf("errno的意思是：%s\n", strerror_l(errno, loc));

//	GNU版本strerror_r函数
//	int buf_size = 100;
//	char *buf = malloc(buf_size);
//	char *res = strerror_r(errno, buf, buf_size);
//	printf("buf是：%s\n", buf);
//	printf("res是：%s\n", res);

//	POSIX版本strerror_r函数
//	int buf_size = 100;
//	char *buf = malloc(buf_size);
//	int res = strerror_r(errno, buf, buf_size);
//	printf("buf是：%s\n", buf);
//	printf("res是：%d\n", res);
	
	return 0;
}
