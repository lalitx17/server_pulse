#include "response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

response_t response_init(status_code_t status_code, char *headers, char *body) {
    response_t resp;
    resp.status_code = status_code;

    char status_text[64];
    switch (status_code) {
    case STATUS_OK:
        strcpy(status_text, "200 OK");
        break;
    case STATUS_CREATED:
        strcpy(status_text, "201 Created");
        break;
    case STATUS_NO_CONTENT:
        strcpy(status_text, "204 No Content");
        break;
    case STATUS_MOVED_PERMANENTLY:
        strcpy(status_text, "301 Moved Permanently");
        break;
    case STATUS_FOUND:
        strcpy(status_text, "302 Found");
        break;
    case STATUS_BAD_REQUEST:
        strcpy(status_text, "400 Bad Request");
        break;
    case STATUS_UNAUTHORIZED:
        strcpy(status_text, "401 Unauthorized");
        break;
    case STATUS_FORBIDDEN:
        strcpy(status_text, "403 Forbidden");
        break;
    case STATUS_NOT_FOUND:
        strcpy(status_text, "404 Not Found");
        break;
    case STATUS_INTERNAL_SERVER_ERROR:
        strcpy(status_text, "500 Internal Server Error");
        break;
    case STATUS_SERVICE_UNAVAILABLE:
        strcpy(status_text, "503 Service Unavailable");
        break;
    default:
        strcpy(status_text, "500 Internal Server Error");
        break;
    }

    resp.status_line = malloc(strlen("HTTP/1.1 ") + strlen(status_text) + 1);
    strcpy(resp.status_line, "HTTP/1.1 ");
    strcat(resp.status_line, status_text);

    if (headers != NULL) {
        resp.headers = malloc(strlen(headers) + 1);
        strcpy(resp.headers, headers);
    } else {
        resp.headers = NULL;
    }

    if (body != NULL) {
        resp.body = malloc(strlen(body) + 1);
        strcpy(resp.body, body);
    } else {
        resp.body = NULL;
    }

    return resp;
}

char *response(response_t *response) {
    size_t total_len = strlen(response->status_line) + 2;

    if (response->headers != NULL) {
        total_len += strlen(response->headers) + 2;
    } else {
        total_len += 2;
    }

    if (response->body != NULL) {
        total_len += strlen(response->body);
    }

    total_len += 1;

    char *response_str = malloc(total_len);
    strcpy(response_str, response->status_line);
    strcat(response_str, "\r\n");

    if (response->headers != NULL) {
        strcat(response_str, response->headers);
        strcat(response_str, "\r\n");
    } else {
        strcat(response_str, "\r\n");
    }

    if (response->body != NULL) {
        strcat(response_str, response->body);
    }

    return response_str;
}

void response_free(response_t *response) {
    free(response->status_line);
    free(response->headers);
    free(response->body);
    free(response);
}