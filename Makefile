CC = gcc
OUTPUT = bin
TARGET = $(OUTPUT)/employee_manager

.PHONY: clean
all: pre $(TARGET)

clean:
	rm -rf $(OUTPUT)/*

pre:
	$(shell [ ! -d $(OUTPUT) ] && mkdir -p $(OUTPUT))

CFLAGS += $(FLAG) -Wall -DDEBUG -std=gnu11 -fPIC -fstack-protector-strong
INCLUDES = -Icommand_parser/ -Icommand_execution/ -Ihash_table/ -Icommon/
OBJS = $(OUTPUT)/hash_table.o $(OUTPUT)/command_execution.o $(OUTPUT)/command_parser.o $(OUTPUT)/main.o

$(OUTPUT)/hash_table.o: hash_table/hash_table.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/command_execution.o: command_execution/command_execution.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/command_parser.o: command_parser/command_parser.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/main.o: main.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/employee_manager: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) -Lbin

