# Notes

Useful observations from the analysis:

```sh
file level0
```

```txt
level0: setuid ELF 32-bit LSB executable, Intel 80386, statically linked, for GNU/Linux 2.6.24, not stripped
```

Security properties shown by the login banner:

```txt
No RELRO        No canary found   NX enabled    No PIE          No RPATH   No RUNPATH   /home/user/level0/level0
```

Important instructions used in `gdb`:

```asm
call   atoi
cmp    eax,0x1a7
call   getegid
call   geteuid
call   setresgid
call   setresuid
call   execv
```

The value `0x1a7` equals `423`.

`strings` returned an input/output error on this VM, so the analysis was performed with runtime tests and `gdb` instead.
