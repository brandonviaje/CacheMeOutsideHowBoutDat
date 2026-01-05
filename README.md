# CacheMeOutsideHowBoutDat

> A personal systems-level project where I build my own Redis-like in-memory data store in **C++**, focusing on performance, networking, and clean architecture.

---

## Overview

This project is a ground-up implementation inspired by **Redis**, designed to deeply understand:

* Low-level networking (TCP sockets)
* Efficient in-memory data structures
* Event-driven server design
* Persistence and durability strategies

The goal is **learning-first**, not feature parity — every component is intentionally built by hand.

---

## Goals

* ✅ Implement a Redis-style TCP server
* ✅ Support core data types (starting simple)
* ✅ Design a clean, scalable C++ project structure
* ⏳ Add persistence (RDB / AOF-style)
* ⏳ Optimize for speed and low latency

---

## Features

### Implemented

* Basic TCP server
* Client request handling
* Command parsing (WIP)

### In Progress

* Key-value store (`SET`, `GET`)
* In-memory hash table
* Connection handling

### Planned

* Expiring keys (TTL)
* Persistence (snapshot + append-only log)
* Event loop (select / poll / epoll)
* Replication (stretch goal)

---

## Project Structure

```text
redis/
├── include/        # Public headers
│   ├── net/        # Socket & networking interfaces
│   ├── core/       # Core data structures
│   └── utils/      # Helpers & utilities
├── src/            # Implementation files
│   ├── net/
│   ├── core/
│   └── main.cpp
├── tests/          # Unit & integration tests
├── CMakeLists.txt
└── README.md
```

---

## Tech Stack

* **Language:** C++17
* **Build System:** CMake
* **Platform:** Linux / WSL (Windows-compatible)
* **Networking:** POSIX sockets

---

## Building & Running

```bash
mkdir build
cd build
cmake ..
make
./redis
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
It is a **learning project** built to explore systems programming concepts.

---

## Roadmap

* [ ] Core KV commands
* [ ] Persistence layer
* [ ] Event loop
* [ ] Performance profiling

---

## Author

Built with curiosity and caffeine ☕

If you're into systems programming, networking, or databases — welcome 👋
