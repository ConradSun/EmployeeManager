CC = gcc
OUTPUT = bin
TARGET = $(OUTPUT)/EmployeeManager

.PHONY: clean
all: pre $(TARGET)

clean:
	rm -rf $(OUTPUT)/*

pre:
	$(shell [ ! -d $(OUTPUT) ] && mkdir -p $(OUTPUT))

CFLAGS += $(FLAG) -Wall -DDEBUG -std=gnu11 -fPIC -fstack-protector-strong
INCLUDES = -ICommandParser/ -IHashTable/
OBJS = $(OUTPUT)/hash_table.o $(OUTPUT)/command_parser.o $(OUTPUT)/main.o

$(OUTPUT)/hash_table.o: HashTable/hash_table.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/command_parser.o: CommandParser/command_parser.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/main.o: main.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/EmployeeManager: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) -Lbin
