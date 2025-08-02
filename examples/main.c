#include "../metrics/metrics.h"
#include "../request/request.h"
#include "../response/response.h"
#include "../server/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

void check_error(int ret, const char *msg);

void root_handler(request_t *request, response_t *response) {
    (void)request;
    send_file(response, "public/home.html");
}

void cpustat_handler(request_t *request, response_t *response) {
    (void)request;
    send_file(response, "public/cpustat.html");
}

void cpustat_json_handler(request_t *request, response_t *response) {
    (void)request;
    char *json = get_cpu_metrics_json();
    send_json(response, json);
    free(json);
}

void memorystat_handler(request_t *request, response_t *response) {
    (void)request;
    send_file(response, "public/memorystat.html");
}

void memorystat_json_handler(request_t *request, response_t *response) {
    (void)request;
    char *json = get_memory_metrics_json();
    send_json(response, json);
    free(json);
}

void diskstat_handler(request_t *request, response_t *response) {
    (void)request;
    send_file(response, "public/diskstat.html");
}

void diskstat_json_handler(request_t *request, response_t *response) {
    (void)request;
    char *json = get_disk_metrics_json();
    send_json(response, json);
    free(json);
}

void check_error(int ret, const char *msg) {
    if (ret < 0) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    int port = PORT;
    if (argc == 2) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port number\n");
            return -1;
        }
    }

    server_t serv;
    if (Server(&serv, port) < 0) {
        fprintf(stderr, "Failed to start server\n");
        return -1;
    }

    add_route(&serv, "GET", "/", root_handler);
    add_route(&serv, "GET", "/cpustat", cpustat_handler);
    add_route(&serv, "GET", "/cpustatjson", cpustat_json_handler);
    add_route(&serv, "GET", "/memorystat", memorystat_handler);
    add_route(&serv, "GET", "/memorystatjson", memorystat_json_handler);
    add_route(&serv, "GET", "/diskstat", diskstat_handler);
    add_route(&serv, "GET", "/diskstatjson", diskstat_json_handler);

    printf("Server started on port %d\n", port);

    while (1) {
        int client_fd = client_accept(&serv);
        if (client_fd < 0) {
            continue;
        }

        char buffer[1024];
        request_t *request = malloc(sizeof(request_t));
        if (!request) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        memset(request, 0, sizeof(request_t));
        int bytes_read = request_init(client_fd, buffer, sizeof(buffer));
        if (bytes_read <= 0) {
            fprintf(stderr,
                    "Failed to read from client or client disconnected\n");
            close(client_fd);
            continue;
        }

        printf("buffer: %s\n", buffer);
        int parse_result = request_parse(buffer, request);
        if (parse_result == 0) {
            check_error(print_request(request), "print request");
        } else {
            fprintf(stderr, "Failed to parse request\n");
            request_destroy(request);
            free(request);
            close(client_fd);
            continue;
        }

        if (server_enqueue_task(&serv, client_fd, request) < 0) {
            fprintf(stderr, "Failed to enqueue task\n");
            request_destroy(request);
            free(request);
            close(client_fd);
            continue;
        }

        request_destroy(request);
        free(request);
    }

    server_close(&serv);
    return 0;
}
