#include "main.h"

int init(size_t port) {

    server_socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if(server_socket_fd < 0){
        printf("Error creating socket\n");
        return EXIT_FAILURE;
    }

    int optval = 1;
    if(setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval)) < 0){
        printf("Error setting socket options\n");
        close(server_socket_fd);
        return EXIT_FAILURE;
    }

    memset(&address,0,sizeof(address));
    address.sin6_family = AF_INET6;
    address.sin6_addr = in6addr_any;
    address.sin6_port = htons(port);

    if(bind(server_socket_fd, (struct sockaddr*)&address,sizeof(address))<0){
        printf("Error binding socket\n");
        close(server_socket_fd);
        return EXIT_FAILURE;
    }

    if(listen(server_socket_fd, 3)<0){
        printf("Error listening on socket");
        close(server_socket_fd);
        return EXIT_FAILURE;
    }

    printf("Server started on port: %ld (IPv6)\n",port);

    socket_fds[0].fd = server_socket_fd;
    socket_fds[0].events = POLLIN;

    return 0;
}