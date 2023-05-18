# Walkthrough

## Reversing

We have a `level1` binary, let's reverse it.

The binary has a main function which uses `gets`... And has a 76 bytes char
array as buffer, that -of course- is susceptible to buffer overflow.

```c
void main(void)
{
    char local_50 [76];

    gets(local_50);
    return;
}
```

There's also a `run` function:
```c
void run(void)
{
    fwrite("Good... Wait what?\n",1,0x13,stdout);
    system("/bin/sh");
    return;
}
```

## Exploit

So, of course, what we need to do here is overwrite the return address of `main`
to the address of `run`, since there's no `PIE` protection, this is easy.

Usually I would create a script on python using the `pwn` library, but we don't
have that on the machine, so let's do a bash script instead.

You can find the script on `./resources/exploit.sh`.

To get the address of the `run` function, just go to ghidra and take the address
of the first instruction, or use a tool like `nm` and search for the `run`
symbol.

So, we need to fill the buffer of `local_50`, since this variable is the first
thing on the function, just above it will be the return address of `main`,
so we need to write 76 bytes + address of `run`, that way we would be
overwriting the return address of `main` with the address of `run`, and
changing the program flow, so `run` gets executed, and we get a shell as
`level2`.

We need to take care of a few things like the endian, but that's all, easy
stuff, just launch the `exploit.sh`.

As a side note to the code, I use FIFOs and not just `echo "payload" | level1`,
because the binary launch `/bin/sh`, an interactive shell, and, when the pipe
buffer ends (all the payload) is sent, `/bin/sh` will read EOF and stops its
execution. That's not what we want, so we have two options:

### cat

We can do something like `cat /tmp/payload - | level1`.

But that's not fun, so...

### FIFOs

We can use a FIFO as the input, so we can send the payload and later attach our
stdin to the FIFO to interact with the binary.

The advantages of this method is we have more control about the binary input,
since FIFOs are files, any process can open it for reading or writing, and we
can send various command outputs to the same FIFO, pipes are limited to the
command that goes before it.

But yead, for this binary we can use the simplier `cat` method.

### Flag

So, launch `exploit.sh`, and in the interactive session run:
```bash
cd /home/user/level2
cat .pass
```
