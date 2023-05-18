#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void p(void)
{
	char local_50 [76];

	fflush(stdout);
	gets(local_50);
	if ((((unsigned int)(__builtin_extract_return_addr (__builtin_return_address (0)))) & 0xb0000000) == 0xb0000000) {
		printf("(%p)\n", __builtin_extract_return_addr (__builtin_return_address (0)));
		exit(1);
	}
	puts(local_50);
	strdup(local_50);
	return;
}

int main(void)
{
	p();
	return 0;
}
