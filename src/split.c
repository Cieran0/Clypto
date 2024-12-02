#include "main.h"

int try_allocate_and_copy(char*** out, size_t k, const char* in, size_t line_start, size_t length) {
    (*out)[k] = (char*)malloc(length + 1);
    if ((*out)[k] == NULL) {
        return 0;
    }
    strncpy((*out)[k], in + line_start, length);
    (*out)[k][length] = '\0';
    return 1;
}

size_t split_lines(const char* in, char delim, char*** out) {
    
    if (in == NULL || out == NULL) {
        return 0;
    }

    if(!in[0]) {
        return 0;
    }

    size_t count = 0;
    size_t i = 1;

    for (; in[i]; i++) {
        if (in[i] == delim && in[i-1] != delim) {
            count++;
        }
    }
    
    if (in[i-1] != delim) {
        count++;
    }

    if(count == 0){
        return 0;
    }

    (*out) = (char**)malloc(count * sizeof(char*));
    if ((*out) == NULL) {
        return 0;
    }

    size_t k = 0;
    size_t line_start = 0;

    for (i = 0; in[i]; i++) {
        if(in[i] != delim) {
            continue;
        }
        
        size_t length = i - line_start;
        
        if (length > 0) {
            if (!try_allocate_and_copy((out), k, in, line_start, length)) {
                for (size_t j = 0; j < k; j++) {
                    free((*out)[j]);
                }
                free((*out));
                return 0;
            }
            k++;
        }
        
        line_start = i + 1;
    }

    size_t length = i - line_start;
    if (length > 0) {
        if (!try_allocate_and_copy((out), k, in, line_start, length)) {
            for (size_t j = 0; j < k; j++) {
                free((*out)[j]);
            }
            free((*out));
            return 0;
        }
    }

    return k + (length > 0 ? 1 : 0);
}