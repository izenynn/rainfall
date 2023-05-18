#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int m;

void v(void)
{
	char local_20c [520];

	fgets(local_20c,512,stdin);
	printf(local_20c);
	if (m == 64) {
		fwrite("Wait what?!\n",1,12,stdout);
		system("/bin/sh");
	}
	return;
}

int main(void)
{
	v();
	return 0;
}
