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
#include <netdb.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 16384

typedef struct thread_context {
    int socket;
} thread_context_t;

#define LINE_SIZE 1024
#define FIELD_SIZE 256

static inline void print(char* str) {
    write(STDERR_FILENO, str, strlen(str));
}

void crash_handler(int signum, siginfo_t *siginfo, void *ucontext) {
    char line[LINE_SIZE + 1] = {0};
    char field[FIELD_SIZE + 1] = {0};

    print("-- BSORD START --\n");

    pthread_t self = pthread_self();
    pthread_getname_np(self, field, FIELD_SIZE);

    snprintf(line, LINE_SIZE, "Thread: %s\n", field);
    print(line);

    switch (signum) {
    case SIGABRT: strcpy(field, "SIGABRT"); break;
    case SIGBUS: strcpy(field, "SIGBUS"); break;
    case SIGFPE: strcpy(field, "SIGFPE"); break;
    case SIGILL: strcpy(field, "SIGILL"); break;
    case SIGSEGV: strcpy(field, "SIGSEGV"); break;
    default:
        snprintf(field, FIELD_SIZE, "%d", signum);
        break;
    }

    snprintf(line, LINE_SIZE, "Signal: %s\n", field);
    print(line);

    unw_context_t context;
    unw_cursor_t cursor;

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    print("Backtrace:\n");

    unw_word_t ip, sp, offset;
    int i = -1;

    while (unw_step(&cursor) > 0) {
        if (i == -1) {
            i++;
            continue;
        }

        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);
        unw_get_proc_name(&cursor, field, FIELD_SIZE, &offset);

        snprintf(line, LINE_SIZE,
                 "  %2d: [0x%016" PRIxPTR "] %s+0x%" PRIxPTR " (0x%016" PRIxPTR ")\n",
                 i, ip, field, offset, sp);
        print(line);

        if (i == 0) {
            unw_word_t rax, rbx, rcx, rdx, rdi, rsi, rbp, rsp;
            unw_word_t r8, r9, r10, r11, r12, r13, r14, r15;

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

    print("-- BSORD END --\n");

    exit(1); // XXX Proper error code
}

bool visited = false;

__attribute__((noinline))
void crash() {
    volatile int i = *(int*) 7;
    (void) i;
}

__attribute__((noinline))
int inner() {
    crash();
    return 0;
}

__attribute__((noinline))
int outer() {
    inner();
    return 0;
}

void* run(void* data) {
    int sock = ((thread_context_t*) data)->socket;
    char* buffer = (char*) malloc(BUFFER_SIZE);

    int opt = 1;
    int err = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*) &opt, sizeof(opt));
    if (err) goto egress;

    int i = 0;

    while (1) {
        ssize_t received = recv(sock, buffer, BUFFER_SIZE, 0);
        if (received < 0) goto egress;

        if (received == 0) {
            printf("server: Connection closed by peer\n");
            goto egress;
        }

        if (!visited && i++ == 2) {
            visited = true;
            outer();
        }

        ssize_t sent = send(sock, buffer, received, 0);
        if (sent < 0) goto egress;
    }

egress:

    if (errno) {
        fprintf(stderr, "server: ERROR! %s\n", strerror(errno));
    }

    close(sock);
    free(buffer);

    return NULL;
}

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

    int port = 9090;

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) goto egress;

    int opt = 1;
    int err = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (err) goto egress;

    struct sockaddr_in addr = (struct sockaddr_in) {
        0,
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
        .sin_port = htons(port)
    };

    err = bind(server_sock, (const struct sockaddr*) &addr, sizeof(addr));
    if (err) goto egress;

    err = listen(server_sock, 1);
    if (err) goto egress;

    printf("server: Listening for connections on port %d\n", port);

    while (1) {
        int sock = accept(server_sock, NULL, NULL);
        if (sock < 0) goto egress;

        printf("server: Connection accepted\n");

        pthread_t* thread = malloc(sizeof(pthread_t));
        thread_context_t* context = malloc(sizeof(thread_context_t));

        *context = (thread_context_t) {
            .socket = sock,
        };

        pthread_create(thread, NULL, &run, (void*) context);
        pthread_setname_np(*thread, "worker");
    }

egress:

    if (errno) {
        fprintf(stderr, "server: ERROR! %s\n", strerror(errno));
    }

    if (server_sock > 0) {
        shutdown(server_sock, SHUT_RDWR);
    }

    return errno;
}
