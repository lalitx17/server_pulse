#Directories
EXAMPLE_DIR := examples
SERVER_DIR := server

#settings
CC := gcc
CFLAGS := -Wall -Wextra -g
INCS := -I$(SERVER_DIR)

EXAMPLE_OBJ := $(EXAMPLE_DIR)/main.o
SERVER_OBJ := $(SERVER_DIR)/server.o

TARGET := server-pulse

all: $(TARGET)

$(SERVER_OBJ) : $(SERVER_DIR)/server.c $(SERVER_DIR)/server.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@


$(EXAMPLE_OBJ) : $(EXAMPLE_DIR)/main.c $(SERVER_DIR)/server.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(TARGET) : $(SERVER_OBJ) $(EXAMPLE_OBJ)
	$(CC) $(CFLAGS) $(INCS) $^ -o $@

.phony: clean
clean:
	rm -f $(TARGET) $(EXAMPLE_OBJ) $(SERVER_OBJ)
