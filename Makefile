INCLUDE_DIRS = -I./src -I./tests
CC = gcc
CFLAGS = -Wall -g $(INCLUDE_DIRS)
LDFLAGS = -lrt
CLIENT1_SRC = src/client1.c
CLIENT2_SRC = src/client2.c
PROTOCOL_SRC = src/protocol.c
TEST_PROTOCOL_SRC = tests/test_protocol.c
TEST_CLIENT1_SRC = tests/test_client1.c
TEST_CLIENT2_SRC = tests/test_client2.c
CLIENT1_BIN = bin/client1
CLIENT2_BIN = bin/client2
TEST_PROTOCOL_BIN = bin/test_protocol
TEST_CLIENT1_BIN = bin/test_client1
TEST_CLIENT2_BIN = bin/test_client2

.PHONY: all
all: clean bin $(CLIENT1_BIN) $(CLIENT2_BIN) test $(LDFLAGS)

bin:
	mkdir -p bin

$(CLIENT1_BIN): $(CLIENT1_SRC) src/protocol.h bin
	$(CC) $(CFLAGS) -o $(CLIENT1_BIN) $(CLIENT1_SRC) $(PROTOCOL_SRC) $(LDFLAGS)

$(CLIENT2_BIN): $(CLIENT2_SRC) src/protocol.h bin
	$(CC) $(CFLAGS) -o $(CLIENT2_BIN) $(CLIENT2_SRC) $(PROTOCOL_SRC) $(LDFLAGS)

$(TEST_PROTOCOL_BIN): $(TEST_PROTOCOL_SRC) src/protocol.h tests/test.h bin
	$(CC) $(CFLAGS) -o $(TEST_PROTOCOL_BIN) $(TEST_PROTOCOL_SRC) $(PROTOCOL_SRC) $(LDFLAGS)

$(TEST_CLIENT1_BIN): $(TEST_CLIENT1_SRC) src/protocol.h tests/test.h bin
	$(CC) $(CFLAGS) -o $(TEST_CLIENT1_BIN) $(TEST_CLIENT1_SRC) $(PROTOCOL_SRC) $(LDFLAGS)

$(TEST_CLIENT2_BIN): $(TEST_CLIENT2_SRC) src/protocol.h tests/test.h bin
	$(CC) $(CFLAGS) -o $(TEST_CLIENT2_BIN) $(TEST_CLIENT2_SRC) $(PROTOCOL_SRC) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(CLIENT1_BIN) $(CLIENT2_BIN) $(TEST_PROTOCOL_BIN) $(TEST_CLIENT1_BIN) $(TEST_CLIENT2_BIN)

.PHONY: client1
client1: $(CLIENT1_BIN) $(LDFLAGS)

.PHONY: client2
client2: $(CLIENT2_BIN) $(LDFLAGS)

.PHONY: test
test: $(TEST_PROTOCOL_BIN) $(TEST_CLIENT1_BIN) $(TEST_CLIENT2_BIN) $(LDFLAGS)
	./$(TEST_PROTOCOL_BIN)
	./$(TEST_CLIENT1_BIN)
	./$(TEST_CLIENT2_BIN)
