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

## Result

~~~
plano: notice: Starting command 'server'
plano: notice: Sleeping for 1 second
server: Listening for connections on port 9090
plano: notice: Running command 'client'
client: Connecting to port 9090
client: Connected
[...]
server: Connection accepted
client: Connecting to port 9090
client: Connected
server: Connection accepted
-- BSORD START --
Signal: SIGSEGV
Backtrace:
   0: ip=0x00007faf7263ea00 sp=0x00007faf725db080 __sigaction [+0x50]
   1: ip=0x00000000004016b4 sp=0x00007faf725dbd50 crash [+0x4]
   2: ip=0x00000000004016db sp=0x00007faf725dbd60 inner [+0xb]
   3: ip=0x00000000004016eb sp=0x00007faf725dbd70 outer [+0xb]
   4: ip=0x0000000000401830 sp=0x00007faf725dbd80 run [+0x140]
   5: ip=0x00007faf7268cdcd sp=0x00007faf725dbe60 start_thread [+0x2cd]
   6: ip=0x00007faf72712630 sp=0x00007faf725dbf00 __clone3 [+0x30]
-- BSORD END --
client: ERROR! Connection reset by peer
client: ERROR! Connection reset by peer
client: ERROR! Connection reset by peer
client: Disconnected
[...]
client: Disconnected
plano: notice: Killing process 1619346 (command 'server')
~~~
