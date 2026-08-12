#include <errno.h>
#include <stdio.h>

int main()
{
	//	errno.h
	printf("errno为%d\n", errno);

	FILE *f = fopen("./nofile", "r");

	printf("errno为%d\n", errno);

	return 0;
}
