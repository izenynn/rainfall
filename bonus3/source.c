#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char buff[152];
	FILE *file;
	
	file = fopen("/home/user/end/.pass","r");
	memset(buff, 0, 152);

	if (file == NULL || argc != 2) {
		return -1;
	}
	fread(buff, 1, 66, file);
	buff[65] = '\0';
	*(buff + atoi(argv[1])) = '\0';
	fread(buff + 66, 1, 65, file);
	fclose(file);

	if (strcmp(buff, argv[1]) == 0)
		execl("/bin/sh", "sh", NULL);
	else
		puts(buff + 66);

	return 0;
}
