#include "http.h"

bool is_get(struct http_request request) {
    return request.is_get;
}


int main(int argc, char const *argv[])
{

    struct http_responder repsonders[] = {
        {
            .condition = is_get,
            .response = serve_static_page
        }
    };

    start_http_server(8000, repsonders, 1);
    
    return 0;
}
