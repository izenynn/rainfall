#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char **argv)
{
	int ret;
	char buff[40];
	int num;

	num = atoi(argv[1]);
	if (num < 10) {
		memcpy(buff, argv[2], num * 4);
		if (num == 1464814662) {
			execl("/bin/sh","sh",0);
		}
		ret = 0;
	} else {
		ret = 1;
	}

	return ret;
}
