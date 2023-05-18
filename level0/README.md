# Walkthrough

Since it's the first walkthrough, let's talk a littel about linux binary
protections...

- NX: The NX (do not execute) bit is a technology used in CPUs that guarantees that certain memory areas (such as the stack and heap) are not executable, and others, such as the code section, cannot be written. It prevents us from using simpler techniques where we wrote shellcode in the stack and execute it.
- ASLR: randomizes the base of the libraries (libc) so that we can't know the memory address of functions of the libc. This avoids the technique Ret2libc, and forces us to have to leak addresses of the same in order to calculate base.
- PIE: like the ASLR, randomizes the base address, but for the binary itselft. This make it difficult to use gadgets or functions of the binary.
- Canario: Normally a random value is generated at program initialization, and inserted at the end of the high risk area where the stack overflows, at the end of each function, before the return, it's checked wheter the canary value has been modified and exists if so.

Let's reverse the binary in ghidra.

It's has a lot of useless stuff, the `main` function, is the only relevant one:
```c
undefined4 main(undefined4 param_1,int param_2)
{
  int iVar1;
  char *local_20;
  undefined4 local_1c;
  __uid_t local_18;
  __gid_t local_14;
  
  iVar1 = atoi(*(char **)(param_2 + 4));
  if (iVar1 == 0x1a7) {
    local_20 = strdup("/bin/sh");
    local_1c = 0;
    local_14 = getegid();
    local_18 = geteuid();
    setresgid(local_14,local_14,local_14);
    setresuid(local_18,local_18,local_18);
    execv("/bin/sh",&local_20);
  }
  else {
    fwrite("No !\n",1,5,(FILE *)stderr);
  }
  return 0;
}
```

Remember you can find a readable version of the binary on the `source.c` file.

For me that is understandable, but let's clean it a little anyway
(in ghidra of course):
```c
int main(int argc,char **argv)
{
    int n;
    char *exec_argv;
    undefined4 local_1c;
    __uid_t euid;
    __gid_t egid;

    n = atoi(argv[1]);
    if (n == 423) {
        exec_argv = strdup("/bin/sh");
        local_1c = 0;
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
```

Basically, it sets the Read UID, Effective UID, and Saved Set-UID to the
effective UID of the binary, who is `level1`.

I don't need to explain that code right? Just run:
```bash
./level0 423
```

And:
```bash
cat /home/user/level1/.pass
```
