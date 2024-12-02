#pragma once

#define _GNU_SOURCE
#include "stdio.h"
#undef _GNU_SOURCE
#include "poll.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "unistd.h"
#include "arpa/inet.h"
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "http.h"
#define MAX_CLIENTS 1024
#define FREE_ARRAY(array, size) for (size_t i = 0; i < size; i++) { free(array[i]); } free(array);

extern size_t server_socket_fd;
extern struct pollfd socket_fds[MAX_CLIENTS];
extern size_t number_of_socket_fds;
extern struct sockaddr_in6 address;

extern struct http_responder* g_responders;
extern size_t g_responders_len;

int init(size_t port);

void poll_loop();
void respond_to_client(int client_socket, const char* buffer);

char* substr(const char* string, size_t start, size_t end);
size_t split_lines(const char* in, char delim, char*** out);

