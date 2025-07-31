#ifndef ROUTE_H
#define ROUTE_H

#include "../request/request.h"
#include "../response/response.h"
#include "../server/server.h"

typedef void (*route_handler_t)(request_t *request, response_t *response);

typedef struct Route {
    char method[8];
    char path[256];
    route_handler_t handler;
} route_t;

int routes_init(route_t **routes, int *capacity);
int add_route(server_t *server, const char *method, const char *path,
              route_handler_t handler);
route_t *route_find(server_t *server, const char *method, const char *path);
void routes_free(route_t *routes);

#endif // ROUTE_H