#Directories
EXAMPLE_DIR := examples
SERVER_DIR := server
REQUEST_DIR := request
METRICS_DIR := metrics
RESPONSE_DIR := response
ROUTES_DIR := route

#settings
CC := gcc
CFLAGS := -Wall -Wextra -g
INCS := -I$(SERVER_DIR) -I$(REQUEST_DIR) -I$(METRICS_DIR) -I$(RESPONSE_DIR) -I$(ROUTES_DIR)

EXAMPLE_OBJ := $(EXAMPLE_DIR)/main.o
SERVER_OBJ := $(SERVER_DIR)/server.o
REQUEST_OBJ := $(REQUEST_DIR)/request.o
METRICS_OBJ := $(METRICS_DIR)/metrics.o
RESPONSE_OBJ := $(RESPONSE_DIR)/response.o
ROUTES_OBJ := $(ROUTES_DIR)/routes.o

TARGET := server-pulse

all: $(TARGET)

$(SERVER_OBJ) : $(SERVER_DIR)/server.c $(SERVER_DIR)/server.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(METRICS_OBJ): $(METRICS_DIR)/metrics.c $(METRICS_DIR)/metrics.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(REQUEST_OBJ) : $(REQUEST_DIR)/request.c $(REQUEST_DIR)/request.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(RESPONSE_OBJ) : $(RESPONSE_DIR)/response.c $(RESPONSE_DIR)/response.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(ROUTES_OBJ) : $(ROUTES_DIR)/route.c $(ROUTES_DIR)/route.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(EXAMPLE_OBJ) : $(EXAMPLE_DIR)/main.c $(SERVER_DIR)/server.h $(REQUEST_DIR)/request.h $(RESPONSE_DIR)/response.h $(ROUTES_DIR)/route.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(TARGET) : $(SERVER_OBJ) $(REQUEST_OBJ) $(METRICS_OBJ) $(RESPONSE_OBJ) $(ROUTES_OBJ) $(EXAMPLE_OBJ)
	$(CC) $(CFLAGS) $(INCS) $^ -o $@

.phony: clean
clean:
	rm -f $(TARGET) $(EXAMPLE_OBJ) $(SERVER_OBJ) $(METRICS_OBJ) $(REQUEST_OBJ) $(RESPONSE_OBJ) $(ROUTES_OBJ)
