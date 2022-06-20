CC = gcc
OUTPUT = ../bin

ifeq ($(shell uname), Darwin)
	LIB = $(OUTPUT)/libem_db.dylib
else
	LIB = $(OUTPUT)/libem_db.so
endif

TARGET = $(LIB)
CFLAGS += $(FLAG) -Wall -std=gnu11 -fstack-protector-strong
INCLUDES = -Ihash_table/ -I../src/common/
LIB_OBJS = $(OUTPUT)/hash_table.o

.PHONY: clean
all: pre $(TARGET)

clean:
	rm -f $(LIB_OBJS)
	rm -f $(TARGET)

pre:
	$(shell [ ! -d $(OUTPUT) ] && mkdir -p $(OUTPUT))

$(OUTPUT)/hash_table.o: hash_table/hash_table.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(LIB): $(LIB_OBJS)
	$(CC) -o $@ $^ $(INCLUDES) $(CFLAGS) -fPIC -shared
