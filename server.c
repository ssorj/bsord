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
#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <libunwind.h>
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
