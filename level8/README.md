# Walkthrough

## Reversing

Okey, this one is big. I cleaned it up a little so a normal human being can
understand what is going on:
```c
undefined4 main(void)
{
  char cVar1;
  char *ret;
  int len;
  uint uVar2;
  bool bVar3;
  undefined uVar4;
  undefined uVar5;
  bool bVar6;
  undefined uVar7;
  byte bVar8;
  char buff [5];
  char local_8b [2];
  char acStack_89 [125];
  byte *command;
  byte *input;
  
  bVar8 = 0;
  do {
    printf("%p, %p \n",auth,service);
    ret = fgets(buff,128,stdin);
    bVar3 = false;
    bVar6 = ret == (char *)0x0;
    if (bVar6) {
      return 0;
    }
    len = 5;
    input = (byte *)buff;
    command = (byte *)"auth ";
    do {
      if (len == 0) break;
      len = len + -1;
      bVar3 = *input < *command;
      bVar6 = *input == *command;
      input = input + (uint)bVar8 * -2 + 1;
      command = command + (uint)bVar8 * -2 + 1;
    } while (bVar6);
    uVar4 = 0;
    uVar7 = (!bVar3 && !bVar6) == bVar3;
    if ((bool)uVar7) {
      auth = (undefined4 *)malloc(4);
      *auth = 0;
      uVar2 = 0xffffffff;
      ret = local_8b;
      do {
        if (uVar2 == 0) break;
        uVar2 = uVar2 - 1;
        cVar1 = *ret;
        ret = ret + (uint)bVar8 * -2 + 1;
      } while (cVar1 != '\0');
      uVar2 = ~uVar2 - 1;
      uVar4 = uVar2 < 0x1e;
      uVar7 = uVar2 == 0x1e;
      if (uVar2 < 0x1f) {
        strcpy((char *)auth,local_8b);
      }
    }
    len = 5;
    input = (byte *)buff;
    command = (byte *)"reset";
    do {
      if (len == 0) break;
      len = len + -1;
      uVar4 = *input < *command;
      uVar7 = *input == *command;
      input = input + (uint)bVar8 * -2 + 1;
      command = command + (uint)bVar8 * -2 + 1;
    } while ((bool)uVar7);
    uVar5 = 0;
    uVar4 = (!(bool)uVar4 && !(bool)uVar7) == (bool)uVar4;
    if ((bool)uVar4) {
      free(auth);
    }
    len = 6;
    input = (byte *)buff;
    command = (byte *)"service";
    do {
      if (len == 0) break;
      len = len + -1;
      uVar5 = *input < *command;
      uVar4 = *input == *command;
      input = input + (uint)bVar8 * -2 + 1;
      command = command + (uint)bVar8 * -2 + 1;
    } while ((bool)uVar4);
    uVar7 = 0;
    uVar4 = (!(bool)uVar5 && !(bool)uVar4) == (bool)uVar5;
    if ((bool)uVar4) {
      uVar7 = (char *)0xfffffff8 < buff;
      uVar4 = acStack_89 == (char *)0x0;
      service = strdup(acStack_89);
    }
    len = 5;
    input = (byte *)buff;
    command = (byte *)"login";
    do {
      if (len == 0) break;
      len = len + -1;
      uVar7 = *input < *command;
      uVar4 = *input == *command;
      input = input + (uint)bVar8 * -2 + 1;
      command = command + (uint)bVar8 * -2 + 1;
    } while ((bool)uVar4);
    if ((!(bool)uVar7 && !(bool)uVar4) == (bool)uVar7) {
      if (auth[8] == 0) {
        fwrite("Password:\n",1,10,stdout);
      }
      else {
        system("/bin/sh");
      }
    }
  } while( true );
}
```

The binary basically looks for a command, there are 4 available commands:
- "`auth `".
- "`reset`".
- "`service`".
- "`login`".

Also note that `buff` has a size of 5, but `fgets` reads 128 bytes, this is
of course intended so we can write to the next two stack arrays...

So, first things first, what we need is the shell that gets executed when
`auth[8] == 0`, so let's focus on that.

Remember that the pointers that are being printend are `auth` and `service`,
will be useful later.

Let's also analyze what does each command:

### auth

```c
auth = (undefined4 *)malloc(4);
*auth = 0;
uVar2 = 0xffffffff;
ret = local_8b; // Defined as `char local_8b[2]`
do {
  if (uVar2 == 0) break;
  uVar2 = uVar2 - 1;
  cVar1 = *ret;
  ret = ret + (uint)bVar8 * -2 + 1;
} while (cVar1 != '\0');
uVar2 = ~uVar2 - 1;
uVar4 = uVar2 < 0x1e;
uVar7 = uVar2 == 0x1e;
if (uVar2 < 0x1f) {
  strcpy((char *)auth,local_8b);
}
```

1. It allocates a 4 byte memory region for the `auth` pointer.
2. Set `auth[0]` to `\0`, understandable.
3. Then it iterates `local_8b` until a `\0` is found, but since is never assigned its values are not initialized.
    1. `uVar2` which starts with an odd value, is decreased every iteration.
4. It checks that `uVar2` is less than 31 after inverting it.
    1. If so, it does an unsafe `strcpy` of `local_8b`

`local_8b` is just past the 5 bytes of `buff`, so we cound overflow the `fgets`
and place whatever we want on `local_8b`, and it will be copied to `auth`.

That `uVar2` condition is basically checking the lenght of the input to be less
than 31, I think.

### reset

```c
free(auth);
```

This one is easy, just frees `auth`.

### service

```c
uVar7 = (char *)0xfffffff8 < buff;
uVar4 = acStack_89 == (char *)0x0;
service = strdup(acStack_89);
```

The first two lines are irrelevant.

The third line dups `acStack_89` into `service`.

This means that anything behind `service` will be copied, because
`service` has a length of 7, so anything behind that will be overflow to the
`acStack_89` variable.

### login

```c
if (auth[8] == 0) {
  fwrite("Password:\n",1,10,stdout);
}
else {
  system("/bin/sh");
}
```

This is what we need to bypass, we need `auth[8]` to be something different
from `\0`, and that's funny because `auth` will never have a space of more than
4 bytes allocated...

## Exploit

But, remember the variables:
```raw
  char buff [5];
  char local_8b [2];
  char acStack_89 [125];
```

Okey... Let's think...

We need, `auth[8]` to have some value, but `auth` malloc only allocated `4`
bytes, `auth` is a pointer to an element of size 4, so `auth[8]` measn:
`8 * 4 = 32` byte, so we need to make sure that there is something `32` bytes past
`*auth`.

So, first we call the "auth" command, so auth will get allocated 4 bytes. Now,
we nee to make sure that there are 28 bytes more, and the last one has
something different from 0.

If we call service with a long argument, we can make sure the `strdup` will
allocate enaught bytes, and since the allocation will occur after the `auth`
allocation, it will be below on the heap...

So, `auth[8]` will be pointing to the `service` `strdup`.

Of course, the `malloc_chunk` structure is gonna be in the middle, but, since
`auth[8]` in the byte 32, and auth has allocated only 4 bytes, that means
we have `32 - 4 = 28` bytes for the `malloc_chunk` structure.

That's more than enought, the `malloc_chunk` struct will end and the duped
string will start, so byte 32 is guaranteed to be something when we `login`.

Let's try it:
```bash
./level8
auth 
serviceaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
login

$ cd /home/user/level9
$ cat .pass
```

*Note: rememeber the `auth ` command has an space in it!

For some reason I struggle automating this level, so, since it's so easy,
I won't do a script.
