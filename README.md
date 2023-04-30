# BSORD

"Blue" screen of router death.

## How it's built

~~~ sh
gcc server.c panic.c -o server -g -O2 -std=c99 -fno-omit-frame-pointer -lunwind -Wall -Wextra
~~~

## The interesting code

[panic.c][panic.c]

## Example output

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
