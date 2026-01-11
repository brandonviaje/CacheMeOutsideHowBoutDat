# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src/utils

# Source files
SERVER_SRC = src/tcp_server.cpp src/utils.cpp src/buffer.cpp
CLIENT_SRC = src/tcp_client.cpp src/utils.cpp src/buffer.cpp 

# Output binaries
SERVER_BIN = server
CLIENT_BIN = client

# Default target
all: $(SERVER_BIN) $(CLIENT_BIN)

# Build server
server: $(SERVER_SRC)
	@$(CXX) $(CXXFLAGS) $^ -o $(SERVER_BIN)

# Build client
client: $(CLIENT_SRC)
	@$(CXX) $(CXXFLAGS) $^ -o $(CLIENT_BIN)

clean:
	@rm -f $(SERVER_BIN) $(CLIENT_BIN)

.PHONY: all clean server client
