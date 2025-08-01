#ifndef RESPONSE_H
#define RESPONSE_H

typedef enum {
    STATUS_OK = 200,
    STATUS_CREATED = 201,
    STATUS_NO_CONTENT = 204,
    STATUS_MOVED_PERMANENTLY = 301,
    STATUS_FOUND = 302,
    STATUS_BAD_REQUEST = 400,
    STATUS_UNAUTHORIZED = 401,
    STATUS_FORBIDDEN = 403,
    STATUS_NOT_FOUND = 404,
    STATUS_INTERNAL_SERVER_ERROR = 500,
    STATUS_SERVICE_UNAVAILABLE = 503
} status_code_t;

typedef struct Response {
    char *status_line;
    status_code_t status_code;
    char *headers;
    char *body;

} response_t;

response_t response_init(status_code_t status_code, char *headers,
                         const char *body);
char *response_string(response_t *response);
void send_error_html(response_t *response, status_code_t status_code,
                     char const *title, char const *message);
void send_html(response_t *response, char const *html_content);
void send_file(response_t *response, char const *file_path);
void send_json(response_t *response, char const *json_content);
void response_free(response_t *response);

#endif // RESPONSE_H
