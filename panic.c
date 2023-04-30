/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#define UNW_LOCAL_ONLY
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <libunwind.h>
#include <libunwind-x86_64.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 256
#define FIELD_SIZE 64

static void print(char* str) {
    write(STDERR_FILENO, str, strlen(str));
}

static void print_backtrace_x86_64(void) {
    char line[LINE_SIZE + 1] = {0};
    char field[FIELD_SIZE + 1] = {0};

    int err;
    unw_context_t context;
    unw_cursor_t cursor;

    err = unw_getcontext(&context);

    if (err) {
        print("Failed getting backtrace: unw_getcontext\n");
        return;
    }

    err = unw_init_local(&cursor, &context);

    if (err) {
        print("Failed getting backtrace: unw_init_local");
        return;
    }

    print("Backtrace:\n");

    unw_word_t ip, sp, offset;
    int i = 0;

    // Advance past the signal handler frame
    unw_step(&cursor);

    while (unw_step(&cursor) > 0) {
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);
        unw_get_proc_name(&cursor, field, FIELD_SIZE, &offset);

        snprintf(line, LINE_SIZE, "  %2d: [0x%016" PRIxPTR "] %s+0x%" PRIxPTR " (0x%016" PRIxPTR ")\n", i, ip, field, offset, sp);
        print(line);

        if (i == 0) {
            unw_word_t rax, rbx, rcx, rdx, rdi, rsi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15;

            unw_get_reg(&cursor, UNW_X86_64_RAX, &rax);
            unw_get_reg(&cursor, UNW_X86_64_RBX, &rbx);
            unw_get_reg(&cursor, UNW_X86_64_RCX, &rcx);
            unw_get_reg(&cursor, UNW_X86_64_RDX, &rdx);
            unw_get_reg(&cursor, UNW_X86_64_RDI, &rdi);
            unw_get_reg(&cursor, UNW_X86_64_RSI, &rsi);
            unw_get_reg(&cursor, UNW_X86_64_RBP, &rbp);
            unw_get_reg(&cursor, UNW_X86_64_RSP, &rsp);
            unw_get_reg(&cursor, UNW_X86_64_R8, &r8);
            unw_get_reg(&cursor, UNW_X86_64_R9, &r9);
            unw_get_reg(&cursor, UNW_X86_64_R10, &r10);
            unw_get_reg(&cursor, UNW_X86_64_R11, &r11);
            unw_get_reg(&cursor, UNW_X86_64_R12, &r12);
            unw_get_reg(&cursor, UNW_X86_64_R13, &r13);
            unw_get_reg(&cursor, UNW_X86_64_R14, &r14);
            unw_get_reg(&cursor, UNW_X86_64_R15, &r15);

            snprintf(line, LINE_SIZE, "      RAX: 0x%016" PRIxPTR "  RDI: 0x%016" PRIxPTR "  R11: 0x%016" PRIxPTR "\n", rax, rdi, r11);
            print(line);
            snprintf(line, LINE_SIZE, "      RBX: 0x%016" PRIxPTR "  RBP: 0x%016" PRIxPTR "  R12: 0x%016" PRIxPTR "\n", rbx, rbp, r12);
            print(line);
            snprintf(line, LINE_SIZE, "      RCX: 0x%016" PRIxPTR "   R8: 0x%016" PRIxPTR "  R13: 0x%016" PRIxPTR "\n", rcx, r8, r13);
            print(line);
            snprintf(line, LINE_SIZE, "      RDX: 0x%016" PRIxPTR "   R9: 0x%016" PRIxPTR "  R14: 0x%016" PRIxPTR "\n", rdx, r9, r14);
            print(line);
            snprintf(line, LINE_SIZE, "      RSI: 0x%016" PRIxPTR "  R10: 0x%016" PRIxPTR "  R15: 0x%016" PRIxPTR "\n", rsi, r10, r15);
            print(line);
        }

        i++;
    }
}

static void panic_handler(int signum, siginfo_t *siginfo, void *ucontext) {
    (void) siginfo;
    (void) ucontext;

    char line[LINE_SIZE + 1] = {0};
    char field[FIELD_SIZE + 1] = {0};

    print("-- PANIC START --\n");

    // Signal

    switch (signum) {
    case SIGABRT: strcpy(field, "SIGABRT"); break;
    case SIGBUS: strcpy(field, "SIGBUS"); break;
    case SIGFPE: strcpy(field, "SIGFPE"); break;
    case SIGILL: strcpy(field, "SIGILL"); break;
    case SIGSEGV: strcpy(field, "SIGSEGV"); break;
    }

    snprintf(line, LINE_SIZE, "Signal: %d (%s)\n", signum, field);
    print(line);

    // Errno

    snprintf(line, LINE_SIZE, "Errno: %d (%s)\n", errno, strerrorname_np(errno));
    print(line);

    // Process

    pid_t pid = getpid();

    snprintf(line, LINE_SIZE, "Process: %d (%s)\n", pid, program_invocation_name);
    print(line);

    // Thread

    pid_t tid = gettid();
    pthread_t self = pthread_self();
    pthread_getname_np(self, field, FIELD_SIZE);

    snprintf(line, LINE_SIZE, "Thread: %d (%s)\n", tid, field);
    print(line);

    // Backtrace

    if (UNW_TARGET_X86_64) {
        print_backtrace_x86_64();
    }

    // Exit code

    snprintf(line, LINE_SIZE, "Exit code: %d\n", -signum);
    print(line);

    print("-- PANIC END --\n");

    exit(-signum);
}

void install_panic_handler(void) {
    struct sigaction sa = {
        .sa_flags = SA_SIGINFO,
        .sa_sigaction = panic_handler,
    };

    sigemptyset(&sa.sa_mask);

    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
}
