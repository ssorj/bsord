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
Process: 1658445 (server)
Thread: 1658449 (worker)
Backtrace:
   0: [0x00000000004014a4] crash+0x4 (0x00007f6af63fedf0)
      RAX: 0x0000000000000000  RDI: 0x00007f6af63ff948  R11: 0x0000000000000246
      RBX: 0x00007f6af0000b70  RBP: 0x00007f6af63fedf0  R12: 0x0000000000000004
      RCX: 0x0000000000000000   R8: 0x0000000000000000  R13: 0x0000000000000003
      RDX: 0x0000000000004000   R9: 0x0000000000000000  R14: 0x00007fff24be1060
      RSI: 0x00007f6af0000b70  R10: 0x0000000000000000  R15: 0x00007f6af5a70000
   1: [0x00000000004014cb] inner+0xb (0x00007f6af63fee00)
   2: [0x00000000004014db] outer+0xb (0x00007f6af63fee10)
   3: [0x0000000000401588] run+0xa8 (0x00007f6af63fee20)
   4: [0x00007f6af648cdcd] start_thread+0x2cd (0x00007f6af63fee60)
   5: [0x00007f6af6512630] __clone3+0x30 (0x00007f6af63fef00)
Exit code: -11
-- PANIC END --
~~~

Then I hacked it into the router:

~~~
-- PANIC START --
Signal: SIGILL
Errno: 0 (0)
Process: 1661445 (skrouterd)
Thread: 1661455 (wrkr_1)
Backtrace:
   0: [0x000000000046a3e3] qd_message_receive+0xf3 (0x00007f8dd2c99f90)
      RAX: 0x0000000000000002  RDI: 0x00007f8de3640c20  R11: 0x00007f8de3603990
      RBX: 0x00007f8dc4070f60  RBP: 0x00007f8dc405b208  R12: 0x00007f8dc408ae48
      RCX: 0x00000000004c1fe8   R8: 0x0000000000000002  R13: 0x00007f8dc409dd48
      RDX: 0x00007f8dc409dd48   R9: 0x0000000000000002  R14: 0x00007f8dc401c688
      RSI: 0x0000000000000000  R10: 0x00007f8de35f3678  R15: 0x0000000000000000
   1: [0x00000000004a8bfe] AMQP_rx_handler.lto_priv.0+0x6e (0x00007f8dd2c99fd0)
   2: [0x000000000045be18] qd_container_handle_event+0x748 (0x00007f8dd2c9a070)
   3: [0x00000000004b17c8] handle+0x58 (0x00007f8dd2c9a0c0)
   4: [0x00000000004b2904] thread_run+0x134 (0x00007f8dd2c9a250)
   5: [0x00007f8de2c8cdcd] start_thread+0x2cd (0x00007f8dd2c9a2a0)
   6: [0x00007f8de2d12630] __clone3+0x30 (0x00007f8dd2c9a340)
Exit code: -4
-- PANIC END --
~~~

Another example from the router:

~~~
-- PANIC START --
Signal: 6 (SIGABRT)
Errno: 11 (EAGAIN)
Process: 1670789 (skrouterd)
Thread: 1670798 (wrkr_0)
Backtrace:
   0: [0x00007f515268ebec] __pthread_kill_implementation+0x11c (0x00007f5142f19be0)
      RAX: 0x0000000000000000  RDI: 0x0000000000197e85  R11: 0x0000000000000246
      RBX: 0x0000000000197e8e  RBP: 0x00007f5142f1b640  R12: 0x0000000000000006
      RCX: 0x00007f515268ebec   R8: 0x00007f5138133b90  R13: 0x000000000190e7c8
      RDX: 0x0000000000000006   R9: 0x0000000000000000  R14: 0x00000000018fefc8
      RSI: 0x0000000000197e8e  R10: 0x0000000000000008  R15: 0x00007f51340b9d08
   1: [0x00007f515263e956] raise+0x16 (0x00007f5142f19ca0)
   2: [0x00007f51526287f4] abort+0xcf (0x00007f5142f19cb0)
   3: [0x000000000042749d] qd_connection_invoke_deferred_impl.part.0.cold+0x5 (0x00007f5142f19df0)
   4: [0x00000000004adcf8] qd_connection_invoke_deferred.part.0+0x38 (0x00007f5142f19e10)
   5: [0x00000000004ae5e7] qd_link_q2_restart_receive+0x47 (0x00007f5142f19e30)
   6: [0x000000000044f386] qdr_tcp_deliver+0x26 (0x00007f5142f19e50)
   7: [0x000000000049bb18] qdr_link_process_deliveries+0xe8 (0x00007f5142f19ee0)
   8: [0x0000000000484878] qdr_connection_process+0x388 (0x00007f5142f19f40)
   9: [0x000000000044f2c9] on_activate+0x39 (0x00007f5142f1a0a0)
  10: [0x00000000004aec65] qd_timer_visit+0x85 (0x00007f5142f1a0b0)
  11: [0x00000000004b1b1d] handle+0x3d (0x00007f5142f1a0c0)
  12: [0x00000000004b2cac] thread_run+0x16c (0x00007f5142f1a250)
  13: [0x00007f515268cdcd] start_thread+0x2cd (0x00007f5142f1a2a0)
  14: [0x00007f5152712630] __clone3+0x30 (0x00007f5142f1a340)
Exit code: -6
-- PANIC END --
~~~

## Notes

* https://crash-utility.github.io/help_pages/bt.html
* https://github.com/cslarsen/libunwind-examples/blob/master/backtrace.cpp
* https://github.com/libunwind/libunwind
* https://manpages.debian.org/testing/manpages-dev/mprotect.2.en.html
* https://www.hpl.hp.com/hosted/linux/mail-archives/libunwind/2004-March/000245.html
* https://www.nongnu.org/libunwind/docs.html
