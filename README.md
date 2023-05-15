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
Signal: 6 (SIGABRT)
Process: 2000049 (server)
Thread: 2000053 (worker)
Registers:
     IP: 0x000000000040151a     SP: 0x00007fbe74ffede0
    RAX: 0x0000000000000000[R] RDI: 0x00000000001e84b1[0] R11: 0x0000000000000246
    RBX: 0x0000000000000009    RBP: 0x00007fbe74ffedf0    R12: 0x0000000000000004
    RCX: 0x00007fbe7508ebec[3]  R8: 0x00000000ffffffff[4] R13: 0x0000000000000003
    RDX: 0x0000000000000006[2]  R9: 0x0000000000000000[5] R14: 0x00007ffe31a60840
    RSI: 0x00000000001e84b5[1] R10: 0x0000000000000008    R15: 0x00007fbe74670000
Backtrace:
    0: 0x00007fbe7503ea00 (0x00007fbe74ffdd40)
    1: 0x00007fbe7508ebec (0x00007fbe74ffea20)
    2: 0x00007fbe7503e956 (0x00007fbe74ffeae0)
    3: 0x00007fbe750287f4 (0x00007fbe74ffeaf0)
    4: 0x00007fbe75082d3e (0x00007fbe74ffec30)
    5: 0x00007fbe7509893c (0x00007fbe74ffed40)
    6: 0x00007fbe7509c0ee (0x00007fbe74ffed50)
    7: 0x00007fbe7509cb98 (0x00007fbe74ffedb0)
    8: 0x000000000040151a (0x00007fbe74ffede0) # addr2line -a 0x40151a -e server
    9: 0x0000000000401569 (0x00007fbe74ffee00)
   10: 0x000000000040161c (0x00007fbe74ffee10)
   11: 0x00007fbe7508cdcd (0x00007fbe74ffee60)
   12: 0x00007fbe75112630 (0x00007fbe74ffef00)
-- PANIC END --
~~~

Then I hacked it into the router:

~~~
-- PANIC START --
Signal: 11 (SIGSEGV)
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
- Add the router version
- More register state?

<!-- Get all the registers and exception state (see the glibc approach) -->
<!-- - SA_RESTART | SA_SIGINFO | SA_ONSTACK - What are these? -->

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
