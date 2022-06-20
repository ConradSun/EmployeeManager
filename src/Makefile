CC = gcc
OUTPUT = ../bin

ifeq ($(shell uname), Darwin)
	LIB = $(OUTPUT)/libem_db.dylib
else
	LIB = $(OUTPUT)/libem_db.so
endif

SRV = $(OUTPUT)/em_server
CLT = $(OUTPUT)/em_client
TARGET = $(SRV) $(CLT)
CFLAGS += $(FLAG) -Wall -std=gnu11 -fstack-protector-strong
INCLUDES = -Icommand_parser/ -Icommand_execution/ -Isocket/ -Icommon/ -I../lib/hash_table/
SRV_OBJS = $(OUTPUT)/command_execution.o $(OUTPUT)/command_parser.o $(OUTPUT)/manager_server.o $(OUTPUT)/main.o
CLT_OBJS = $(OUTPUT)/manager_client.o

.PHONY: clean
all: pre $(TARGET)

clean:
	rm -f $(SRV_OBJS) $(CLT_OBJS)
	rm -f $(TARGET)

pre:
	$(shell [ ! -d $(OUTPUT) ] && mkdir -p $(OUTPUT))

$(OUTPUT)/command_execution.o: command_execution/command_execution.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/command_parser.o: command_parser/command_parser.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/manager_server.o: socket/manager_server.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/main.o: main.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(SRV): $(SRV_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)


$(OUTPUT)/manager_client.o: socket/manager_client.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(CLT): $(CLT_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

