#include "response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

response_t response_init(status_code_t status_code, char *headers,
                         const char *body) {
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

char *response_string(response_t *response) {
    size_t total_len = strlen(response->status_line) + 2;

    if (response->headers != NULL) {
        total_len += strlen(response->headers) + 4;
    } else {
        total_len += 4;
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
        strcat(response_str, "\r\n\r\n");
    } else {
        strcat(response_str, "\r\n\r\n");
    }

    if (response->body != NULL) {
        strcat(response_str, response->body);
    }

    return response_str;
}

void send_html(response_t *response, char const *html_content) {
    if (html_content == NULL || response == NULL) {
        return;
    }

    char headers[512];
    snprintf(headers, sizeof(headers),
             "Content-Type: text/html; charset=utf-8\r\n"
             "Content-Length: %zu\r\n"
             "Cache-Control: no-cache",
             strlen(html_content));

    response_free(response);

    *response = response_init(STATUS_OK, headers, html_content);
}

void send_error_html(response_t *response, status_code_t status_code,
                     const char *title, const char *message) {
    if (response == NULL) {
        return;
    }

    if (title == NULL)
        title = "Error";
    if (message == NULL)
        message = "An error occurred";

    char html_content[512];
    snprintf(html_content, sizeof(html_content),
             "<html><head><title>%s</title></head>"
             "<body><h1>%d - %s</h1><p>%s</p></body></html>",
             title, status_code, title, message);

    char headers[256];
    snprintf(headers, sizeof(headers),
             "Content-Type: text/html; charset=utf-8\r\n"
             "Content-Length: %zu\r\n"
             "Cache-Control: no-cache",
             strlen(html_content));

    response_free(response);

    *response = response_init(status_code, headers, html_content);
}

void send_file(response_t *response, char const *file_path) {
    if (file_path == NULL || response == NULL) {
        return;
    }

    if (access(file_path, F_OK) != 0) {
        send_error_html(
            response, STATUS_NOT_FOUND, "File Not Found",
            "The requested file could not be found on this server.");
        return;
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        send_error_html(response, STATUS_INTERNAL_SERVER_ERROR,
                        "File Open Error",
                        "Failed to open the requested file.");
        return;
    }

    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0) {
        fclose(file);
        send_error_html(response, STATUS_INTERNAL_SERVER_ERROR,
                        "File Stat Error", "Failed to get file information.");
        return;
    }

    char *file_content = malloc(file_stat.st_size + 1);
    if (file_content == NULL) {
        fclose(file);
        send_error_html(response, STATUS_INTERNAL_SERVER_ERROR,
                        "Memory Allocation Error",
                        "Failed to allocate memory for file content.");
        return;
    }

    size_t bytes_read = fread(file_content, 1, file_stat.st_size, file);
    file_content[bytes_read] = '\0';
    fclose(file);

    char *content_type = "application/octet-stream";
    char *ext = strrchr(file_path, '.');
    if (ext != NULL) {
        ext++;
        if (strcmp(ext, "html") == 0) {
            content_type = "text/html; charset=utf-8";
        } else if (strcmp(ext, "css") == 0) {
            content_type = "text/css; charset=utf-8";
        } else if (strcmp(ext, "js") == 0) {
            content_type = "application/javascript; charset=utf-8";
        } else if (strcmp(ext, "json") == 0) {
            content_type = "application/json; charset=utf-8";
        } else if (strcmp(ext, "xml") == 0) {
            content_type = "application/xml; charset=utf-8";
        } else if (strcmp(ext, "txt") == 0) {
            content_type = "text/plain; charset=utf-8";
        } else if (strcmp(ext, "png") == 0) {
            content_type = "image/png";
        } else if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) {
            content_type = "image/jpeg";
        } else if (strcmp(ext, "gif") == 0) {
            content_type = "image/gif";
        } else if (strcmp(ext, "svg") == 0) {
            content_type = "image/svg+xml";
        } else if (strcmp(ext, "ico") == 0) {
            content_type = "image/x-icon";
        } else if (strcmp(ext, "webp") == 0) {
            content_type = "image/webp";
        } else if (strcmp(ext, "mp4") == 0) {
            content_type = "video/mp4";
        }
    }

    char headers[512];
    snprintf(headers, sizeof(headers),
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "Cache-Control: no-cache",
             content_type, bytes_read);

    response_free(response);

    *response = response_init(STATUS_OK, headers, file_content);
}

void response_free(response_t *response) {
    if (!response) {
        return;
    }

    if (response->status_line) {
        free(response->status_line);
        response->status_line = NULL;
    }
    if (response->headers) {
        free(response->headers);
        response->headers = NULL;
    }
    if (response->body) {
        free(response->body);
        response->body = NULL;
    }
}