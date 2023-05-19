#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void m(void *param_1,int param_2,char *param_3,int param_4,int param_5)
{
	puts("Nope");
	return;
}

void n(void)
{
	system("/bin/cat /home/user/level7/.pass");
	return;
}

int main(int argc,char **argv)
{
	char *__dest;
	void (**ppcVar1)(void);

	__dest = (char *)malloc(64);
	ppcVar1 = (void (**)(void))malloc(4);
	*ppcVar1 = m;
	strcpy(__dest,argv[1]);
	(**ppcVar1)();
	return 0;
}
