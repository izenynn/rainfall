#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int language;

/*
 * This binary was a mess... I recoded it entirely... (x.x)
 */

void greetuser(char *user)
{
	char buff[76]; // Function reserves 88 bytes but `buff` starts at -0x4c

	if (language == 1)
		strcpy(buff, "Hyvää päivää ");
	else if (language == 2)
		strcpy(buff, "Goedemiddag! ");
	else if (language == 0)
		strcpy(buff, "Hello ");
	strcat(buff, user);
	puts(buff);
}

int main(int argc, char *argv[])
{
	char buff[76]; // But only 72 are used
	char *lang;

	lang = NULL;
	if (argc == 3) {
		memset(buff, 0, 76);
		strncpy(buff, argv[1], 40);
		strncpy(buff + 40, argv[2], 32);
		lang = getenv("LANG");
		if (lang) {
			if (memcmp(lang, "fi", 2) == 0) {
				language = 1;
			} else if (memcmp(lang, "nl", 2) == 0) {
				language = 2;
			}
		}
		greetuser(buff);
	}
	return 0;
}
