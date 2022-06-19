CC = gcc
OUTPUT = bin

ifeq ($(shell uname), Darwin)
	LIB = $(OUTPUT)/libem_db.dylib
else
	LIB = $(OUTPUT)/libem_db.so
endif

SRV = $(OUTPUT)/em_server
CLT = $(OUTPUT)/em_client
TARGET = $(LIB) $(SRV) $(CLT)

.PHONY: clean
all: pre $(TARGET)

clean:
	rm -rf $(OUTPUT)/*

pre:
	$(shell [ ! -d $(OUTPUT) ] && mkdir -p $(OUTPUT))

CFLAGS += $(FLAG) -Wall -std=gnu11 -fstack-protector-strong
INCLUDES = -Icommand_parser/ -Icommand_execution/ -Ihash_table/ -Isocket/ -Icommon/
LIB_OBJS = $(OUTPUT)/hash_table.o
SRV_OBJS = $(OUTPUT)/command_execution.o $(OUTPUT)/command_parser.o $(OUTPUT)/manager_server.o $(OUTPUT)/server_main.o
CLT_OBJS = $(OUTPUT)/manager_client.o

$(OUTPUT)/hash_table.o: hash_table/hash_table.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(LIB): $(LIB_OBJS)
	$(CC) -o $@ $^ $(INCLUDES) $(CFLAGS) -Lbin -fPIC -shared


$(OUTPUT)/command_execution.o: command_execution/command_execution.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/command_parser.o: command_parser/command_parser.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/manager_server.o: socket/manager_server.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/server_main.o: socket/server_main.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(SRV): $(SRV_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB) -Lbin


$(OUTPUT)/manager_client.o: socket/manager_client.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(CLT): $(CLT_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) -Lbin

