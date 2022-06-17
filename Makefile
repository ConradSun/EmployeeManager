CC = gcc
OUTPUT = bin
TARGET = $(OUTPUT)/em_server $(OUTPUT)/em_client

.PHONY: clean
all: pre $(TARGET)

clean:
	rm -rf $(OUTPUT)/*

pre:
	$(shell [ ! -d $(OUTPUT) ] && mkdir -p $(OUTPUT))

CFLAGS += $(FLAG) -Wall -DDEBUG -std=gnu11 -fPIC -fstack-protector-strong
INCLUDES = -Icommand_parser/ -Icommand_execution/ -Ihash_table/ -Isocket/ -Icommon/
SRV_OBJS = $(OUTPUT)/hash_table.o $(OUTPUT)/command_execution.o $(OUTPUT)/command_parser.o $(OUTPUT)/manager_server.o $(OUTPUT)/server_main.o
CLT_OBJS = $(OUTPUT)/manager_client.o

$(OUTPUT)/hash_table.o: hash_table/hash_table.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/command_execution.o: command_execution/command_execution.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/command_parser.o: command_parser/command_parser.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/manager_server.o: socket/manager_server.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/server_main.o: socket/server_main.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/em_server: $(SRV_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) -Lbin


$(OUTPUT)/manager_client.o: socket/manager_client.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/em_client: $(CLT_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) -Lbin

