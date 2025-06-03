#ifndef REQUEST_H
#define REQUEST_H

#include <stdlib.h>

typedef struct header{
    char *name;
    char *value;
} header_t;

typedef struct Request {
    char *method;
    char *url;
    char *version;
    int n_headers;
    header_t *headers;
    char *body;
} request_t;

int request_init(int client_fd, char *buffer, size_t buffer_size);
int request_parse(const char *buffer, request_t *request);
int request(int client_fd, char *buffer, size_t buffer_size, request_t *request);
void request_destroy(request_t *request);


#endif //REQUEST_H
