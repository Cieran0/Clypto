#include "main.h"

char* substr(const char* string, size_t start, size_t end) {
    if(!string) {
        return NULL;
    }

    size_t length = strlen(string);

    if(length == 0 && start == 0 && end == 0) {
        char* output = (char*)malloc(1);
        output[0] = '\0';
        return output;
    }
    
    if(start > end || start >= length || end >= length) {
        return NULL;
    }

    size_t size = end - start + 1;

    char* output = (char*)malloc(size + 1);
    memcpy(output, string+start, size);
    output[size]= '\0';

    return output;
}