#Directories
EXAMPLE_DIR := examples
SERVER_DIR := server
REQUEST_DIR := request
METRICS_DIR := metrics

#settings
CC := gcc
CFLAGS := -Wall -Wextra -g
INCS := -I$(SERVER_DIR) -I$(REQUEST_DIR) -I$(METRICS_DIR)

EXAMPLE_OBJ := $(EXAMPLE_DIR)/main.o
SERVER_OBJ := $(SERVER_DIR)/server.o
REQUEST_OBJ := $(REQUEST_DIR)/request.o
METRICS_OBJ := $(METRICS_DIR)/metrics.o

TARGET := server-pulse

all: $(TARGET)

$(SERVER_OBJ) : $(SERVER_DIR)/server.c $(SERVER_DIR)/server.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(METRICS_OBJ): $(METRICS_DIR)/metrics.c $(METRICS_DIR)/metrics.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(REQUEST_OBJ) : $(REQUEST_DIR)/request.c $(REQUEST_DIR)/request.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(EXAMPLE_OBJ) : $(EXAMPLE_DIR)/main.c $(SERVER_DIR)/server.h $(REQUEST_DIR)/request.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(TARGET) : $(SERVER_OBJ) $(REQUEST_OBJ) $(EXAMPLE_OBJ)
	$(CC) $(CFLAGS) $(INCS) $^ -o $@

.phony: clean
clean:
	rm -f $(TARGET) $(EXAMPLE_OBJ) $(SERVER_OBJ) $(METRICS_OBJ) $(REQUEST_OBJ)
