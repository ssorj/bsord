# BSORD

## How it's built

~~~ sh
gcc server.c -o server -g -O2 -std=c99 -fno-omit-frame-pointer -lunwind
~~~

## The interesting code

[server.c](server.c)

~~~ c
XXX
~~~

Setup:

~~~ c
int main(size_t argc, char** argv) {
    struct sigaction sa = {
        0,
        .sa_flags = SA_SIGINFO,
        .sa_sigaction = crash_handler,
    };

    sigemptyset(&sa.sa_mask);

    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);

    // ...
}
~~~

## Example output

The local test output:

~~~
~~~

I hacked this into the router:

~~~
-- PANIC START --
Signal: SIGILL
Errno: 0 (0)
Process: 1646757 (skrouterd)
Thread: 1646766 (wrkr_0)
Backtrace:
   0: [0x000000000046a3e3] qd_message_receive+0xf3 (0x00007fc704b5ff90)
      RAX: 0x0000000000000000  RDI: 0x00007fc714b76c20  R11: 0x00007fc714b39990
      RBX: 0x00007fc6f8000f40  RBP: 0x00007fc6f405af48  R12: 0x00007fc6f8026148
      RCX: 0x00000000004c1fe8   R8: 0x0000000000000002  R13: 0x00007fc6f8039908
      RDX: 0x00007fc6f8039908   R9: 0x00000007fc6f8039  R14: 0x00007fc6f4019208
      RSI: 0x0000000000000000  R10: 0x00007fc714b29678  R15: 0x0000000000000000
   1: [0x00000000004a8bfe] AMQP_rx_handler.lto_priv.0+0x6e (0x00007fc704b5ffd0)
   2: [0x000000000045be18] qd_container_handle_event+0x748 (0x00007fc704b60070)
   3: [0x00000000004b17c8] handle+0x58 (0x00007fc704b600c0)
   4: [0x00000000004b2840] thread_run+0x70 (0x00007fc704b60250)
   5: [0x00007fc71428cdcd] start_thread+0x2cd (0x00007fc704b602a0)
   6: [0x00007fc714312630] __clone3+0x30 (0x00007fc704b60340)
Exit code: -4
-- PANIC END --
~~~
