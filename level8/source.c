#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int *auth;
int *service;

int main(void)
{
	char cVar1;
	char *ret;
	int len;
	unsigned int uVar2;
	bool bVar3;
	bool uVar4;
	bool uVar5;
	bool bVar6;
	bool uVar7;
	char bVar8;
	char buff [5];
	char local_8b [2];
	char acStack_89 [125];
	char *command;
	char *input;
	
	bVar8 = 0;
	do {
		printf("%p, %p \n",auth,service);
		ret = fgets(buff,128,stdin);
		bVar3 = false;
		bVar6 = ret == (char *)0x0;
		if (bVar6) {
			return 0;
		}
		len = 5;
		input = (char *)buff;
		command = (char *)"auth ";
		do {
			if (len == 0) break;
			len = len + -1;
			bVar3 = *input < *command;
			bVar6 = *input == *command;
			input = input + (unsigned int)bVar8 * -2 + 1;
			command = command + (unsigned int)bVar8 * -2 + 1;
		} while (bVar6);
		uVar4 = 0;
		uVar7 = (!bVar3 && !bVar6) == bVar3;
		if ((bool)uVar7) {
			auth = (int *)malloc(4);
			*auth = 0;
			uVar2 = 0xffffffff;
			ret = local_8b;
			do {
				if (uVar2 == 0) break;
				uVar2 = uVar2 - 1;
				cVar1 = *ret;
				ret = ret + (unsigned int)bVar8 * -2 + 1;
			} while (cVar1 != '\0');
			uVar2 = ~uVar2 - 1;
			uVar4 = uVar2 < 0x1e;
			uVar7 = uVar2 == 0x1e;
			if (uVar2 < 0x1f) {
				strcpy((char *)auth,local_8b);
			}
		}
		len = 5;
		input = (char *)buff;
		command = (char *)"reset";
		do {
			if (len == 0) break;
			len = len + -1;
			uVar4 = *input < *command;
			uVar7 = *input == *command;
			input = input + (unsigned int)bVar8 * -2 + 1;
			command = command + (unsigned int)bVar8 * -2 + 1;
		} while ((bool)uVar7);
		uVar5 = 0;
		uVar4 = (!(bool)uVar4 && !(bool)uVar7) == (bool)uVar4;
		if ((bool)uVar4) {
			free(auth);
		}
		len = 6;
		input = (char *)buff;
		command = (char *)"service";
		do {
			if (len == 0) break;
			len = len + -1;
			uVar5 = *input < *command;
			uVar4 = *input == *command;
			input = input + (unsigned int)bVar8 * -2 + 1;
			command = command + (unsigned int)bVar8 * -2 + 1;
		} while ((bool)uVar4);
		uVar7 = 0;
		uVar4 = (!(bool)uVar5 && !(bool)uVar4) == (bool)uVar5;
		if ((bool)uVar4) {
			uVar7 = (char *)0xfffffff8 < buff;
			uVar4 = acStack_89 == (char *)0x0;
			service = (int *)strdup(acStack_89);
		}
		len = 5;
		input = (char *)buff;
		command = (char *)"login";
		do {
			if (len == 0) break;
			len = len + -1;
			uVar7 = *input < *command;
			uVar4 = *input == *command;
			input = input + (unsigned int)bVar8 * -2 + 1;
			command = command + (unsigned int)bVar8 * -2 + 1;
		} while ((bool)uVar4);
		if ((!(bool)uVar7 && !(bool)uVar4) == (bool)uVar7) {
			if (auth[8] == 0) {
				fwrite("Password:\n",1,10,stdout);
			}
			else {
				system("/bin/sh");
			}
		}
	} while( true );

	return 0;
}
