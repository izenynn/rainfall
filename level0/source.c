#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define _GNU_SOURCE
#include <unistd.h>

int main(int argc, char *argv[])
{
	int n;
	char *exec_argv;
	// undefined4 local_1c; // Useless
	uid_t euid;
	gid_t egid;

	n = atoi(argv[1]);
	if (n == 423) {
		exec_argv = strdup("/bin/sh");
		// local_1c = 0; // Useless
		egid = getegid();
		euid = geteuid();
		setresgid(egid,egid,egid);
		setresuid(euid,euid,euid);
		execv("/bin/sh",&exec_argv);
	} else {
		fwrite("No !\n",1,5,(FILE *)stderr);
	}

	return 0;
}
