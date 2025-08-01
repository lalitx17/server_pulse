#include "route.h"
#include "../server/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int routes_init(route_t **routes, int *capacity) {
    if (!routes || !capacity) {
        return -1;
    }

    *capacity = 10;
    *routes = malloc(*capacity * sizeof(route_t));
    if (!*routes) {
        return -1;
    }

    memset(*routes, 0, *capacity * sizeof(route_t));
    return 0;
}

int add_route(server_t *server, const char *method, const char *path,
              route_handler_t handler) {
    if (!server || !method || !path || !handler) {
        return -1;
    }

    if (server->routes_count >= server->routes_capacity) {
        int new_capacity = server->routes_capacity * 2;
        route_t *new_routes =
            realloc(server->routes, new_capacity * sizeof(route_t));
        if (!new_routes) {
            return -1;
        }
        server->routes = new_routes;
        server->routes_capacity = new_capacity;
    }

    route_t *new_route = &server->routes[server->routes_count];

    memset(new_route, 0, sizeof(route_t));

    strncpy(new_route->method, method, sizeof(new_route->method) - 1);
    new_route->method[sizeof(new_route->method) - 1] = '\0';

    strncpy(new_route->path, path, sizeof(new_route->path) - 1);
    new_route->path[sizeof(new_route->path) - 1] = '\0';

    new_route->handler = handler;
    server->routes_count++;

    printf("Added route: %s %s (total routes: %d)\n", new_route->method,
           new_route->path, server->routes_count);

    return 0;
}

route_t *route_find(server_t *server, const char *method, const char *path) {
    if (!server || !method || !path) {
        return NULL;
    }

    printf("Looking for route: %s %s\n", method, path);

    for (int i = 0; i < server->routes_count; i++) {
        route_t *route = &server->routes[i];
        printf("  Checking route %d: '%s' '%s'\n", i, route->method,
               route->path);

        if (strcmp(route->method, method) == 0 &&
            strcmp(route->path, path) == 0) {
            printf("  Found matching route!\n");
            return route;
        }
    }

    printf("  No matching route found\n");
    return NULL;
}

void routes_free(route_t *routes) {
    if (routes) {
        free(routes);
    }
}