#define _GNU_SOURCE
#include "main.h"
#include "http.h"

struct http_header get_header(const char* header) {

    const static struct http_header FAILED = {
        .name = NULL,
        .value = NULL
    };
    
    if(header == NULL) {
        return FAILED;
    }

    size_t name_end = 0;
    size_t size = strlen(header);
    for (size_t i = 0; i < size; i++)
    {
        if(header[i] == ':') {
            name_end=i;
            break;
        }
    }
    

    if(name_end == 0) {
        return FAILED;
    }
    
    char* name = substr(header, 0, name_end-1);
    char* value = substr(header, name_end+2, size-1);


    struct http_header result = {
        .name = name,
        .value = value,
        .next = NULL
    };


    return result;
}

void add_header(struct http_header_list* header_list, struct http_header header) {
    size_t header_size = strlen(header.name)+ 2 + strlen(header.value);
    
    if(header_list->head == NULL) {
        header_list->head = (struct http_header*)malloc(sizeof(struct http_header));
        header_list->tail = header_list->head;

        *(header_list->head) = header;
        header_list->size+=header_size;
        header_list->count++;
        return;
    }

    header_list->tail->next = (struct http_header*)malloc(sizeof(struct http_header));
    *(header_list->tail->next) = header;
    header_list->tail = header_list->tail->next;
    header_list->size+=header_size;
    header_list->count++;
    return;
}

struct http_header_list get_headers(const char* headers_raw) {
    struct http_header_list headers = {.head = NULL, .tail = NULL, .count = 0, .size = 0 };

    size_t header_len = strlen(headers_raw);
    size_t start = 0;
    for (size_t i = 2; i < header_len; i++)
    {
        if(headers_raw[i-1] == '\r' && headers_raw[i] == '\n') {
            char* header_raw = substr(headers_raw, start, i-2);
            struct http_header header = get_header(header_raw);
            add_header(&headers, header);
            start = i + 1;
            free(header_raw);
        }
    }

    return headers;
}

struct http_response http_not_found_error() {
    //TODO: should probably not be a http_response, just a raw string we send over the socket.
    char* body; 
    asprintf(&body,"404 Not Found");
    char* status_line; 
    asprintf(&status_line,"HTTP/1.1 404 Not Found");

    struct http_header_list headers = get_headers("Content-Type: text/plain; charset=UTF-8\r\nContent-Length: 13\r\nConnection: close");

    struct http_response error = {
        .status_line = status_line,
        .body = body,
        .headers = headers,
    };

    return error;
}

char* http_response_to_string(struct http_response response) {
    size_t headers_len = response.headers.size + (response.headers.count*2) + 2;
    size_t total_size =  headers_len + strlen(response.body) + strlen(response.status_line) + 2;
    char* response_as_string = (char*)malloc(total_size+1);
    if(response_as_string == NULL) {
        return NULL;
    }
    response_as_string[total_size] = 0;
    

    struct http_header* current = response.headers.head;
    char* location = response_as_string;

    int written = sprintf(location, "%s\r\n", response.status_line);
    location += written;

    while (current != NULL)
    {
        written = sprintf(location, "%s: %s\r\n", current->name, current->value);
        current = current->next;
        location += written;
    }

    written = sprintf(location, "\r\n");
    location += written;

    sprintf(location, "%s", response.body);
    response_as_string[total_size] = 0;

    return response_as_string;
}

struct http_response serve_static_page(struct http_request request) {

    //TODO: allow to change directory

    size_t path_len = strlen(request.path);
    if(path_len == 1) {
        request.path = "/index.html";
        path_len = 11;
    }

    char* file_path = (char*)malloc(path_len+2);
    file_path[0] = '.';
    file_path[path_len+1] = '\0';

    memcpy(file_path+1, request.path, path_len);


    FILE* fd = fopen(file_path, "r");
    if(fd == NULL) {
        free(file_path);
        return http_not_found_error();
    }
    free(file_path);


    fseek(fd, 0, SEEK_END);
    size_t file_size = ftell(fd);
    rewind(fd);

    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        fclose(fd);
        return http_not_found_error(); // Should be internal server error?
    }


    size_t bytes_read = fread(buffer, 1, file_size, fd);
    if (bytes_read != file_size) {
        free(buffer);
        fclose(fd);
        return http_not_found_error(); // Should be internal server error?
    }

    buffer[file_size] = '\0';

    struct http_header_list headers = {};
    char* content_size;
    asprintf(&content_size, "Content-Length: %ld", bytes_read); 

    add_header(&headers, get_header("Content-Type: text/html"));
    add_header(&headers, get_header(content_size));

    free(content_size);

    char* status_line;
    asprintf(&status_line, "HTTP/1.1 200 OK");

    struct http_response response = {
        .body = buffer,
        .status_line = status_line,
        .headers = headers
    };

    return response;
}

void http_free_headers(struct http_header_list* headers) {
    if(headers == NULL) {
        return;
    }

    struct http_header* next = headers->head;
    while (next != NULL) {
        struct http_header* temp = next;
        next = next->next;
        free(temp->name);
        free(temp->value);
        free(temp);
    }
    
    headers->head = NULL;
    headers->tail = NULL;
    headers->count = 0;
    headers->size = 0;
}

void http_free_request(struct http_request request) {
    free(request.body);
    free(request.path);
    free(request.request_line);

    http_free_headers(&request.headers);
}

void http_free_response(struct http_response response) {
    free(response.body);
    free(response.status_line);

    http_free_headers(&response.headers);
}

struct http_request http_string_to_request(const char* string) {
    const static struct http_request FAILED = {
        .body = NULL,
        .headers = {},
        .request_line = NULL,

        .is_get = NULL,
        .path = NULL,
    };

    const char* body = strstr(string, "\r\n\r\n");

    if(body == NULL) {
        printf("No body found\n");
        return FAILED;
    }

    char* request_end = strstr(string, "\r\n");

    if(request_end == NULL) {
        printf("No request found\n");
        return FAILED;
    }

    char* request_line = substr(string, 0, (request_end - string)-1);

    size_t header_end = body - string - 1;
    size_t header_start = (request_end - string)+2;

    char* headers_raw = substr(string, header_start, header_end);
    body+=4;

    size_t body_last_index = strlen(body) - 1;
    if(body_last_index == -1) {
        body_last_index = 0;
    }

    char* body_heap = substr(body, 0, body_last_index);

    struct http_header_list headers = get_headers(headers_raw);
    free(headers_raw);


    bool is_get = (request_line[0] == 'G');

    const char* path_start = strstr(request_line, " ")+1;
    const char* path_end = strstr(path_start, " ");

    size_t path_start_pos = path_start - request_line;
    size_t path_end_pos = path_end - request_line;

    char* path = substr(request_line, path_start_pos, path_end_pos-1);

    struct http_request request = {
        .body = body_heap,
        .headers = headers,
        .request_line = request_line,

        .is_get = is_get,
        .path = path,
    };

    return request;
}

#undef _GNU_SOURCE