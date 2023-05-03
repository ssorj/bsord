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
Signal: 4 (SIGILL)
Errno: 11 (EAGAIN)
Process: 1687946 (skrouterd)
Thread: 1687955 (wrkr_1)
Backtrace:
   0: [0x00007f0d21a3ea00] __sigaction+0x50 (0x00007f0d118fdec0)
      RAX: 0x0000000000000000  RDI: 0x00007f0d118fd6e0  R11: 0x0000000000000040
      RBX: 0x00007f0d0806b008  RBP: 0x00007f0d118febf0  R12: 0x0000000000017fca
      RCX: 0x0000000000000000   R8: 0x0000000000000000  R13: 0x0000000000000000
      RDX: 0x0000000000000000   R9: 0x0000000000000073  R14: 0x0000000000000019
      RSI: 0x00007f0d118fd3b0  R10: 0xc2a7a2ce2ada9a00  R15: 0x0000000001d0fa70
   1: [0x000000000046cf61] qd_message_stream_data_append+0xa1 (0x00007f0d118febb0)
      RAX: 0x0000000000000019  RDI: 0x00007f0d0c045ac8  R11: 0x0000000000000246
      RBX: 0x00007f0d0806b008  RBP: 0x00007f0d118febf0  R12: 0x0000000000017fca
      RCX: 0x00000000000001e0   R8: 0x00007f0d118feb40  R13: 0x0000000000000000
      RDX: 0x00007f0d0806b994   R9: 0x0000000000000010  R14: 0x0000000000000019
      RSI: 0x00007f0d118febf0  R10: 0x0000000000000000  R15: 0x0000000001d0fa70
   2: [0x000000000044c93d] handle_incoming+0x15d (0x00007f0d118febe0)
      RAX: 0x0000000000000019  RDI: 0x00007f0d0c045ac8  R11: 0x0000000000000246
      RBX: 0x00007f0d0806b008  RBP: 0x0000000001ce5580  R12: 0x0000000000017fca
      RCX: 0x00000000000001e0   R8: 0x00007f0d118feb40  R13: 0x0000000000000000
      RDX: 0x00007f0d0806b994   R9: 0x0000000000000010  R14: 0x0000000001ce5580
      RSI: 0x00007f0d118febf0  R10: 0x0000000000000000  R15: 0x0000000001d0fa70
   3: [0x000000000044e195] handle_connection_event+0x575 (0x00007f0d118fec70)
   4: [0x00000000004b1c99] handle+0x99 (0x00007f0d118ff0c0)
   5: [0x00000000004b2dcc] thread_run+0x16c (0x00007f0d118ff250)
   6: [0x00007f0d21a8cdcd] start_thread+0x2cd (0x00007f0d118ff2a0)
   7: [0x00007f0d21b12630] __clone3+0x30 (0x00007f0d118ff340)
-- PANIC END --
~~~

## Todo

- Eliminate snprintf usage
- Get all the registers and exception state (see the glibc approach)
- Better unwind error handling
- SA_RESTART | SA_SIGINFO | SA_ONSTACK - What are these?
- Make sure the "main" frame registers are the ones I really want to display
  - Or display all the frames' registers?
- Print addr2line command help
- Can I get the executable name?

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
