#pragma once
#include "stdbool.h"
#include "unistd.h"

struct http_header
{
    char* name;
    char* value;
    struct http_header* next;
};

struct http_header_list
{
    struct http_header* head;
    struct http_header* tail;
    size_t size;
    size_t count;
};

struct http_request
{
    char* request_line;
    struct http_header_list headers;
    char* body;

    bool is_get;
    char* path;
};

struct http_response
{
    char* status_line;
    struct http_header_list headers;
    char* body;
};


struct http_responder {
    bool (*condition)(struct http_request request);
    struct http_response (*response)(struct http_request request);
};

int start_http_server(size_t port, struct http_responder* responders, size_t responders_len);

struct http_header get_header(const char* header);
struct http_header_list get_headers(const char* headers_raw);
struct http_response http_not_found_error();
char* http_response_to_string(struct http_response response);
struct http_response serve_static_page(struct http_request request);
void http_free_request(struct http_request request);
void http_free_response(struct http_response response);
struct http_request http_string_to_request(const char* string);