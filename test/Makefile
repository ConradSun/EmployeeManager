CC = gcc
CXX = g++
OUTPUT = ../bin

TEST = $(OUTPUT)/test
TARGET = $(TEST)
CFLAGS = $(FLAG) -Wall -std=gnu11 -DUNIT_TEST -fstack-protector-strong -fprofile-arcs -ftest-coverage
CXXFLAGS = -std=c++11 -stdlib=libc++ -Wall -DUNIT_TEST -fprofile-arcs -ftest-coverage
INCLUDES = -I../src/database_manager/ -I../src/command_parser/ -I../src/command_execution/ 
INCLUDES += -I../src/socket/ -I../src/common/ -I../lib/hash_table/
OBJS = $(OUTPUT)/database_manager.o $(OUTPUT)/command_execution.o $(OUTPUT)/command_parser.o 
OBJS += $(OUTPUT)/manager_server.o $(OUTPUT)/manager_client.o $(OUTPUT)/hash_table.o
OBJS += $(OUTPUT)/parser_test.o $(OUTPUT)/socket_test.o $(OUTPUT)/database_test.o $(OUTPUT)/execution_test.o
OBJS += $(OUTPUT)/main.o

.PHONY: clean
all: pre $(TARGET)

clean:
	rm -rf $(OUTPUT)

pre:
	$(shell [ ! -d $(OUTPUT) ] && mkdir -p $(OUTPUT))

$(OUTPUT)/database_manager.o: ../src/database_manager/database_manager.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/command_execution.o: ../src/command_execution/command_execution.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/command_parser.o: ../src/command_parser/command_parser.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/manager_server.o: ../src/socket/manager_server.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/manager_client.o: ../src/socket/manager_client.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)

$(OUTPUT)/hash_table.o: ../lib/hash_table/hash_table.c
	$(CC) -o $@ -c $^ $(INCLUDES) $(CFLAGS)


$(OUTPUT)/database_test.o: database_test.cpp
	$(CXX) -o $@ -c $^ $(INCLUDES) $(CXXFLAGS)

$(OUTPUT)/execution_test.o: execution_test.cpp
	$(CXX) -o $@ -c $^ $(INCLUDES) $(CXXFLAGS)

$(OUTPUT)/parser_test.o: parser_test.cpp
	$(CXX) -o $@ -c $^ $(INCLUDES) $(CXXFLAGS)

$(OUTPUT)/socket_test.o: socket_test.cpp
	$(CXX) -o $@ -c $^ $(INCLUDES) $(CXXFLAGS)

$(OUTPUT)/main.o: main.cpp
	$(CXX) -o $@ -c $^ $(INCLUDES) $(CXXFLAGS)

$(TEST): $(OBJS)
	$(CXX) -o $@ $^  $(CXXFLAGS) -lgtest


