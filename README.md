# BSORD

## How it's built

~~~ sh
gcc server.c -o server -g -O2 -std=c99 -fno-omit-frame-pointer -lunwind
~~~

## The interesting code

[server.c](server.c)

~~~ c
#include <libunwind.h>

static inline void print(char* str) {
    write(STDERR_FILENO, str, strlen(str));
}

void crash_handler(int signum, siginfo_t *siginfo, void *ucontext) {
    char field[256] = {0};

    print("-- BSORD START --\n");

    print("Signal: ");

    switch (signum) {
    case SIGBUS:
        print("SIGBUS\n");
        break;
    case SIGFPE:
        print("SIGFPE\n");
        break;
    case SIGILL:
        print("SIGILL\n");
        break;
    case SIGSEGV:
        print("SIGSEGV\n");
        break;
    default:
        snprintf(field, sizeof(field) - 1, "%d\n", signum);
        print(field);
        break;
    }

    unw_context_t context;
    unw_cursor_t cursor;

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    print("Backtrace:\n");

    unw_word_t ip, sp, offset;
    char buffer[1024] = {0};
    int i = 0;

    while (unw_step(&cursor) > 0) {
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);
        unw_get_proc_name(&cursor, field, sizeof(field) - 1, &offset);

        snprintf(buffer, 1024 - 1, "  %2d: ip=0x%016" PRIxPTR " sp=0x%016" PRIxPTR " %s [+0x%" PRIxPTR "]\n", i++, ip, sp, field, offset);
        print(buffer);
    }

    print("-- BSORD END --\n");

    exit(1); // XXX Proper error code
}
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

[...]
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
