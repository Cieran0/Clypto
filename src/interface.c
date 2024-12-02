#include "main.h"

size_t server_socket_fd = 0;
struct pollfd socket_fds[MAX_CLIENTS];
size_t number_of_socket_fds = 1;
struct sockaddr_in6 address = {};

struct http_responder* g_responders;
size_t g_responders_len;

int start_http_server(size_t port, struct http_responder* responders, size_t responders_len) {
    
    if(init(port) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    g_responders = responders;
    g_responders_len = responders_len;
    
    poll_loop();

    return 0;
}
