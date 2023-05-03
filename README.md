# BSORD

A "blue" screen of router death.

To build it:

~~~ sh
gcc server.c panic.c -o server -g -O2 -std=c99 -fno-omit-frame-pointer -lunwind -Wall -Wextra
~~~

The interesting code:

[panic.c](panic.c)

Running the test scenario:

~~~
$ ./plano run
~~~

The local test output:

~~~
-- PANIC START --
Signal: 11 (SIGSEGV)
Errno: 0 (0)
Process: 1688054 (server)
Thread: 1688058 (worker)
Backtrace:
   0: [0x00007ff38503ea00] __sigaction+0x50 (0x00007ff384ffe100)
      RAX: 0x0000000000000000  RDI: 0x00007ff384ffd910  R11: 0x0000000000000000
      RBX: 0x00007ff380000b70  RBP: 0x00007ff384ffedf0  R12: 0x0000000000000004
      RCX: 0x0000000000000000   R8: 0x0000000000000000  R13: 0x0000000000000003
      RDX: 0x0000000000000000   R9: 0x0000000000000073  R14: 0x00007ffef0a78e10
      RSI: 0x00007ff384ffd5e0  R10: 0x0000000000000000  R15: 0x00007ff384670000
   1: [0x00000000004014a4] crash+0x4 (0x00007ff384ffedf0)
      RAX: 0x0000000000000000  RDI: 0x0000000000000001  R11: 0x0000000000000246
      RBX: 0x00007ff380000b70  RBP: 0x00007ff384ffedf0  R12: 0x0000000000000004
      RCX: 0x0000000000000000   R8: 0x0000000000000000  R13: 0x0000000000000003
      RDX: 0x0000000000004000   R9: 0x0000000000000000  R14: 0x00007ffef0a78e10
      RSI: 0x00007ff380000b70  R10: 0x0000000000000000  R15: 0x00007ff384670000
   2: [0x00000000004014ce] inner+0xe (0x00007ff384ffee00)
      RAX: 0x0000000000000000  RDI: 0x0000000000000001  R11: 0x0000000000000246
      RBX: 0x00007ff380000b70  RBP: 0x00007ff384ffee00  R12: 0x0000000000000004
      RCX: 0x0000000000000000   R8: 0x0000000000000000  R13: 0x0000000000000003
      RDX: 0x0000000000004000   R9: 0x0000000000000000  R14: 0x00007ffef0a78e10
      RSI: 0x00007ff380000b70  R10: 0x0000000000000000  R15: 0x00007ff384670000
   3: [0x00000000004014eb] outer+0xb (0x00007ff384ffee10)
   4: [0x0000000000401598] run+0xa8 (0x00007ff384ffee20)
   5: [0x00007ff38508cdcd] start_thread+0x2cd (0x00007ff384ffee60)
   6: [0x00007ff385112630] __clone3+0x30 (0x00007ff384ffef00)
-- PANIC END --
~~~

Then I hacked it into the router:

~~~
-- PANIC START --
Signal: 11 (SIGSEGV)
Errno: 11 (EAGAIN)
Process: 1995599 (skrouterd)
Thread: 1995609 (wrkr_0)
Registers:
     IP: 0x0000000000457d48     SP: 0x00007f0806c3ab30
    RAX: 0x4545454545454545[R] RDI: 0x00007f07fc0071c8[0] R11: 0x0000000000000001
    RBX: 0x00007f07fc0071c8    RBP: 0x00007f07fc0071c8    R12: 0x000000004545f43d
    RCX: 0x0000000000000001[3]  R8: 0x0000000000000000[4] R13: 0x00007f07f80af9d4
    RDX: 0x0000000000000001[2]  R9: 0x0000000000000075[5] R14: 0x00007f0806c3abf0
    RSI: 0x00007f0806c3abf0[1] R10: 0x00007f07fc008462    R15: 0x000000004545f43d
Backtrace:
    0: 0x00007f081623ea00 (0x00007f0806c39e40)
    1: 0x0000000000457d48 (0x00007f0806c3ab30) # addr2line -a 0x457d48 -e skrouterd
    2: 0x000000000046e2c4 (0x00007f0806c3ab90)
    3: 0x000000000044eb3d (0x00007f0806c3abe0)
    4: 0x0000000000450395 (0x00007f0806c3ac70)
    5: 0x00000000004b3069 (0x00007f0806c3b0c0)
    6: 0x00000000004b419c (0x00007f0806c3b250)
    7: 0x00007f081628cdcd (0x00007f0806c3b2a0)
    8: 0x00007f0816312630 (0x00007f0806c3b340)
-- PANIC END --
~~~

## Todo

- Eliminate snprintf usage (?)
- Get all the registers and exception state (see the glibc approach)
- Better unwind error handling
- SA_RESTART | SA_SIGINFO | SA_ONSTACK - What are these?

## Notes

* https://crash-utility.github.io/help_pages/bt.html
* https://fossies.org/linux/ruby/vm_dump.c
* https://github.com/cslarsen/libunwind-examples/blob/master/backtrace.cpp
* https://github.com/libunwind/libunwind
* https://manpages.debian.org/testing/manpages-dev/mprotect.2.en.html
* https://www.hpl.hp.com/hosted/linux/mail-archives/libunwind/2004-March/000245.html
* https://www.nongnu.org/libunwind/docs.html

* https://lemire.me/blog/2023/05/01/under-linux-libsegfault-and-addr2line-are-underrated/
* https://www.marcusfolkesson.se/blog/libsegfault/
* https://sourceware.org/pipermail/glibc-cvs/2022q1/076117.html
* <https://sourceware.org/git/gitweb.cgi?p=glibc.git;h=65ccd641bacea33be23d51da737c2de7543d0f5e>

* https://github.com/stass/libsegfault/blob/master/libsegfault.c
* https://github.com/stass/libsegfault/tree/master
