#include "request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *trim_whitespace(char *str) {
    char *end;

    if (*str == ' ' || *str == '\t')
        str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;

    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n'))
        end--;

    *(end + 1) = '\0';
    return str;
}

int request_init(int client_fd, char *buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0)
        return -1;

    ssize_t bytes_read = read(client_fd, buffer, buffer_size);

    if (bytes_read < 0) {
        perror("error reading from client");
        return -1;
    }

    buffer[bytes_read] = '\0';
    return (int)bytes_read;
}

int request_parse(const char *buffer, request_t *request) {
    if (!buffer || !request) {
        perror("invalid buffer or request");
        return -1;
    }

    memset(request, 0, sizeof(request_t));
    char *newBuf = strdup(buffer);

    if (!newBuf) {
        perror("Memory allocation failed");
        return -1;
    }

    char *line = strtok(newBuf, "\r\n");

    const char *method = strtok(line, " ");
    const char *url = strtok(NULL, " ");
    const char *version = strtok(NULL, "\r\n");

    if (!method || !url || !version) {
        perror("invalid request");
        free(newBuf);
        return -1;
    }

    request->method = strdup(method);
    request->url = strdup(url);
    request->version = strdup(version);

    free(newBuf);

    if (!request->method || !request->url || !request->version) {
        perror("Memory allocation failed");
        return -1;
    }

    int header_count = 0;

    const char *header_start = strstr(buffer, "\r\n") + 2;
    const char *header_end = strstr(header_start, "\r\n\r\n");

    if (!header_end) {
        header_end = buffer + strlen(buffer);
    }

    const char *ptr = header_start;

    while (ptr < header_end) {
        const char *line = strstr(ptr, "\r\n");
        if (!line || line >= header_end)
            break;

        if (memchr(ptr, ':', line - ptr)) {
            header_count++;
        }
        ptr = line + 2;
    }

    if (header_count > 0) {
        request->headers = calloc(header_count, sizeof(header_t));
        if (!request->headers) {
            perror("Memory allocation failed");
            return -1;
        }
    }

    request->n_headers = 0;
    ptr = header_start;

    // parse headers
    while (ptr < header_end && request->n_headers < header_count) {
        const char *line_end = strstr(ptr, "\r\n");
        if (!line_end || line_end >= header_end)
            break;

        int line_length = line_end - ptr;
        char *this_line = malloc(line_length + 1);
        strncpy(this_line, ptr, line_length);
        this_line[line_length] = '\0';

        char *colon = strchr(this_line, ':');
        if (colon) {
            *colon = '\0';
            char *name = trim_whitespace(this_line);
            char *value = trim_whitespace(colon + 1);

            request->headers[request->n_headers].name = strdup(name);
            request->headers[request->n_headers].value = strdup(value);

            if (request->headers[request->n_headers].name &&
                request->headers[request->n_headers].value) {
                request->n_headers++;
            }
        }

        free(this_line);
        ptr = line_end + 2;
    }

    // parse body
    const char *body_start = strstr(buffer, "\r\n\r\n");
    int content_length = -1;
    if (body_start) {
        body_start += 4;

        for (int i = 0; i < request->n_headers; i++) {
            if (strcasecmp(request->headers[i].name, "Content-Length") == 0) {
                content_length = atoi(request->headers[i].value);
                break;
            }
        }
    }

    if (content_length > 0) {
        request->body = malloc(content_length + 1);
        if (request->body) {
            memset(request->body, 0, content_length + 1);
            strncpy(request->body, body_start, content_length);
            request->body[content_length] = '\0';
        }
    } else if (body_start) {
        request->body = strdup(body_start);
    } else {
        request->body = strdup("");
    }

    return 0;
}

int print_request(request_t *Request) {
    if (!Request) {
        printf("Request is NULL\n");
        return -1;
    }

    printf("Method: %s\n", Request->method ? Request->method : "(null)");
    printf("URL: %s\n", Request->url ? Request->url : "(null)");
    printf("Version: %s\n", Request->version ? Request->version : "(null)");

    printf("Headers (%d):\n", Request->n_headers);
    for (int i = 0; i < Request->n_headers; i++) {
        printf("  %s: %s\n",
               Request->headers[i].name ? Request->headers[i].name : "(null)",
               Request->headers[i].value ? Request->headers[i].value
                                         : "(null)");
    }

    printf("Body: %s\n", Request->body ? Request->body : "(null)");
    return 0;
}

void request_destroy(request_t *request) {
    if (!request)
        return;

    free(request->method);
    free(request->url);
    free(request->version);

    for (int i = 0; i < request->n_headers; i++) {
        free(request->headers[i].name);
        free(request->headers[i].value);
    }
    free(request->headers);
    free(request->body);
}
