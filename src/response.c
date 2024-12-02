#include "main.h"

void respond_to_client(int client_socket, const char* buffer) {

    struct http_request request = http_string_to_request(buffer);

    if(request.body == NULL || request.path == NULL || request.request_line == NULL) {
        return; // Failed to get request
    }

    struct http_response response;
    bool got_valid_response = false;

    for (size_t i = 0; i < g_responders_len && !got_valid_response; i++)
    {
        if(g_responders->condition(request)) {
            response = g_responders->response(request);
            got_valid_response = true;
        }
    }
    
    if(!got_valid_response){
        response = http_not_found_error();
    }

    char* response_as_string = http_response_to_string(response);

    send(client_socket, response_as_string, strlen(response_as_string), 0);

    free(response_as_string);

    http_free_request(request);
    http_free_response(response);
    
    return;
} 