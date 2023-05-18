# Walkthrough

## Reversing

The binary has three functions, let's clean up them:
```c
void main(void)
{
    p();
    return;
}

void p(void)
{
    uint unaff_retaddr;
    char local_50 [76];

    fflush(stdout);
    gets(local_50);
    if ((unaff_retaddr & 0xb0000000) == 0xb0000000) {
        printf("(%p)\n",unaff_retaddr);
        _exit(1);
    }
    puts(local_50);
    strdup(local_50);
    return;
}

void FUN_08048390(void)
{
    (*(code *)(undefined *)0x0)();
    return;
}
```

Obviusly this is another buffer overflow, with an offset of 76, like before,
but now we don't have a "run" function...

And the `FUN_08048390` seems irrelevant, maybe some kind of hint... Hhhmmm...

This is gonna be fun, let's do it.

## Exploit

First things first, that `uint unaff_retaddr` is the way ghidra show us the
return address that was pushod on to the stack, so the offset of the return
address is gonna by:
- `char[76] -> 76 bytes`.

Just like before, but, we need to bypass that `if` condition, so the first byte
can't be a `b`, which means, the return address can't be in the stack.

We could just enter some shellcode on the buffer and overwrite the return
address, but I'll be pointing to the stack... Sad.

But, remember the `strdup`, the code is duping the buffer to the heap, and
since the binary doesn't have `PIE`... the heap address returned by `dup` will
be the same always.

So, get you debugger, place a breakpoint after the `strdup`, and check `$eax`
with something like `p/x $eax`, that's the return address.

Next is finding the offset of the return address, open your debugger, place a
breakpoint after the `gets`, since the buffer is 76 bytes long, enter 76 'A',
and print the stack with `x/200x $esp`, check how many bytes more you need
to reach the return address (4 in this case).

So, our offset is 80 bytes, now let's go and get some random shellcode for
`/bin/sh` in [shell-storm](https://shell-storm.org/shellcode/index.html).

We make a `exploit.sh` and execute it! (Check it on `./resources/exploit.sh`)

And now the usual stuff:
```bash
cd /home/user/level3
cat .pass
```
