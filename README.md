# CacheMeOutsideHowBoutDat

> A personal systems-level project where I build my own in-memory data store in **C++**, focusing on performance, networking, and clean architecture.

---

## Overview

This project is a ground-up implementation inspired by **Redis**, designed to deeply understand:

* Low-level networking (TCP sockets)
* Efficient in-memory data structures
* Event-driven server design
* Persistence and durability strategies

The goal is **learning-first**, not feature parity. 

---

## Goals

* Implement Redis-style TCP server
* Support core data types
* Design a clean, scalable C++ project structure
* Add persistence (RDB / AOF-style)
* Optimize for speed and low latency

---

## Features

### Implemented

* Basic TCP server
* Client request handling
* Command parsing
* Key-value store (`SET`, `GET`)
* In-memory hash table
* Connection handling
* Event loop (poll)

### In Progress

* Sorted set data structure (`ZADD`, `ZRANGE`, `ZREM`)
* Command parsing for sorted set operations
* Efficient score-based ordering
* Memory management and item eviction strategies
* Handling edge cases and error responses for new commands

### Planned

* Expiring keys (TTL)
* Persistence (snapshot + append-only log)
* Replication (stretch goal)

---

## Tech Stack

* **Language:** C++17
* **Build System:** CMake
* **Platform:** Linux / WSL (Windows-compatible)
* **Networking:** POSIX sockets

---

## Building & Running

### 1. Build the project
```bash
make
```
### 2. Run the server
```bash
./server
```

### 3. Run the client
```bash
./client
```

Send a single command directly:

```bash
./client "SET key value"
./client "GET key"
```

### Example workflow:

```bash
# Terminal 1: start server
./server

# Terminal 2: connect with client
./client
> SET name GOAT
> GET name
GOAT
```

---

## Learning Focus

This project emphasizes:

* Memory ownership & lifetimes
* Socket I/O and buffering
* Performance-aware design
* Writing production-style C++

---

## Testing

Testing will include:

* Unit tests for data structures
* Integration tests for commands
* Manual testing via `redis-cli`-style clients

---

## Inspiration

* Redis internals
* UNIX network programming
* High-performance server design

---

## Disclaimer

This is **not** a production-ready Redis replacement.
It is a **pet project** built to explore systems programming concepts.

---

## Author

Built with curiosity and caffeine ☕

If you're into systems programming, networking, or databases: welcome 👋






