#include<errno.h>
#include<stdio.h>


int main() {
//	errno.h
//	volatile int errno;
	printf("errno为%d\n", errno);
	
	FILE *f = fopen("./nofile", "r");

	printf("errno为%d\n", errno);
	
	return 0;
}
