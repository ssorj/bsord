#define SETSIG(sa, sig, func) \
    {    memset( &sa, 0, sizeof( struct sigaction ) ); \
         sa.sa_sigaction = func; \
         sa.sa_flags = SA_SIGINFO; \
        sigaction(sig, &sa, 0L); \
    }

// --

struct sigaction sa;
SETSIG(sa, SIGILL, signal_handler);


    // strcat(buf, symbol);
    // strcat(buf, "\n");

    // while (unw_step(&cursor) > 0) {
    //     unw_get_reg(&cursor, UNW_REG_IP, &ip);
    //     unw_get_reg(&cursor, UNW_REG_SP, &sp);

    //     // printf("ip = %lx, sp = %lx\n", (long) ip, (long) sp);
    // }

// --

    // // What is this?
    // snprintf(field, sizeof(field) - 1, "Address: 0x%016" PRIxPTR "\n", siginfo->si_addr);
    // print(field);
