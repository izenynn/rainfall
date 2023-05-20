# Walkthrough

## Reversing

```c
int main(int argc,char **argv)
{
  int iVar1;
  undefined4 *puVar2;
  byte bVar3;
  undefined4 local_98 [16];
  undefined local_57;
  char local_56 [66];
  FILE *local_14;

  bVar3 = 0;
  local_14 = fopen("/home/user/end/.pass","r");
  puVar2 = local_98;
  for (iVar1 = 33; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + (uint)bVar3 * -2 + 1;
  }
  if ((local_14 == (FILE *)0x0) || (argc != 2)) {
    iVar1 = -1;
  }
  else {
    fread(local_98,1,66,local_14);
    local_57 = 0;
    iVar1 = atoi(argv[1]);
    *(undefined *)((int)local_98 + iVar1) = 0;
    fread(local_56,1,65,local_14);
    fclose(local_14);
    iVar1 = strcmp((char *)local_98,argv[1]);
    if (iVar1 == 0) {
      execl("/bin/sh","sh",0);
    }
    else {
      puts(local_56);
    }
    iVar1 = 0;
  }
  return iVar1;
}
```

Okey, the last one, let's see what they got. `B)`

Is a small binary, pretty simple, it even opens the `.pass` file for us in
the first line, thanks!

First, `ghidra` says that `local_98` is `16` bytes long... Error, by looking
at the disassembly its `65` bytes.

It then sets `local_98` first `33` bytes to `0`.

Then it makes some basics checks to ensure the file opened succesfully.

It calls `fread` and reads `66` characters from the `.pass` file and
stores them in `local_98`.

Probably `local_57` and `local_56` are just part of `local_98` but `ghidra`
splited them for some reason, so `local_98` may be bigger then.

It then sets `local_57` to `0`, what a shame, because it's just above `local_98`,
so then indeed `local_98` would be `66` bytes lenght (being `local_57` part of it),
and this lines just make sures the strings ends in `\0`.

No exploits until now... Let's continue...

Then it pass `argv[1]` to `atoi`, and sets `local_98 + atoi(argv[1])` to `0`.

And then it calls `fread` again, and again, it reads the next `65` bytes
from `.pass`, into `local_56`.

So now the contenst of the file are both in `local_98` and `local_56`, but,
`local_98` maybe truncated if we input something smaller than `65` as `argv[1]`.

So now it closes the files and compares `local_98` with `argv[1]`, if the
result is `0`, it opens a shell for us, if not, it prints `local_56`...

## Exploit

Of course I tried to just run it with some random argument so it prints
`local_56`, but seems like the password is on `local_98`, and none was left
when `fread` readed the next `65` bytes... So we will need to bypass that
first condition.

We need `local_98` to be equal to `argv[1]`... We could bruteforce `argv[1]`,
but that's not the intended path. So the easiest way to do it will be using
that `local_98 + atoi(argv[1]) = 0`, to put the first element of `local_98`
to `0`, but then our argument would be `"0"`, and the comparation will fail.

Do you rememer your piscine `atoi`? I do, if the string its empty or has
non digits characters... It will return 0! So, passing an empty string, we can
make `local_98` be empty, and our argument would be empty too!

```bash
./bonus3 ""
```

Cool, I like this level, it's original.

GG rainfall, well played.
