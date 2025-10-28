
# AutoComplete_system

Smart Autocomplete System is a lightweight C++ project that provides fast, in-memory prefix suggestions using a Trie data structure and a small REST API. A tiny web frontend demonstrates the live suggestion behavior and communicates with the C++ backend.

## Table of contents

- [Overview](#overview)
- [Features](#features)
- [Repository layout](#repository-layout)
- [Build & Run (Quickstart)](#build--run-quickstart)
- [API Endpoints](#api-endpoints)
- [Internals](#internals)
- [Data files & format](#data-files--format)
- [Testing](#testing)
- [Extending & Contributing](#extending--contributing)
- [Assumptions & Notes](#assumptions--notes)
- [License](#license)

## Overview

This repository implements a small, fast autocomplete engine in C++:

- A Trie-based index for prefix search and top-k suggestions.
- Merges a base dictionary and user-specific history and boosts suggestions using frequency counts.
- A lightweight REST API (C++ server) exposes suggestion endpoints used by a single-page frontend under `frontend/`.

Use cases: IDE-like word completion, search box suggestions, or small local services needing low-latency prefix queries.

## Features

- Fast prefix lookup using a Trie (memory-backed).
- Merge of base dictionaries and per-user history with simple frequency boosting.
- Small REST API for querying suggestions and updating user history.
- Minimal demo frontend to exercise the API.

## Repository layout

- `include/` — bundled third-party headers (asio, crow, etc.).
- `src/` — main project sources (Trie, TrieNode, Web API server).
- `frontend/` — demo single-page UI (`index.html`, `app.js`, `style.css`).
- `data/` — dictionaries and user data (`data/dictionaries/`, `data/user_data/`).
- `tests/` — small test programs and helper utilities.
- `makefile` — build script.
- `build/` — (optional) build output (may be created by `make`).

## Build & Run (Quickstart)

Prerequisites:

- A C++17-capable compiler (e.g. g++ 7+).
- `make` and `pthread` support (the makefile uses `-pthread`).

Build:

```bash
make
```

Notes on the produced binary:

- Many repositories put the built binary either at `./autocomplete_system` or under `build/`. If `make` creates `build/`, check there. If the binary is not found, list files with `ls -la` and check the `makefile` for the exact output name.

Run (example):

```bash
# try either of these depending on your makefile
./autocomplete_system
# or
./build/autocomplete_system
```

Open the demo frontend:

- Open `frontend/index.html` in a browser, or
- If the server serves the UI over HTTP, point your browser to the server host/port shown in the server log output.

## API Endpoints

Inspect `src/WebAPI.cpp` for exact route paths and request/response shapes. Typical endpoints you can expect:

- `GET /suggest?prefix=<prefix>&k=<k>` — returns top-k suggestions for `prefix` (JSON array/object).
- `POST /user_history` — add/update entries in user history (JSON payload).

Exact JSON structures are defined in `src/WebAPI.cpp`; open that file to confirm required fields and HTTP verbs.

## Internals

- Trie implementation is split across `src/Trie.cpp` and `src/TrieNode.cpp`.
  - `TrieNode::autoComplete` performs traversal and collects top-k suggestions (priority selection / DFS).
  - `TrieNode::getAllWithPrefix` enumerates completions for a given prefix.
- `src/Trie.cpp` contains higher-level logic to load dictionaries, merge with user history, and apply boosting to ranks.
- The server layer in `src/WebAPI.cpp` adapts HTTP requests to trie queries and handles user-history updates.

Edge cases handled (typical):

- Empty prefix — either return empty suggestions or a default hot-list.
- Large k values — your trie traversal may cap results; check `k` handling in `TrieNode::autoComplete`.
- Non-ASCII inputs — ensure your dictionary and trie support UTF-8 if needed.

## Data files & format

- Base dictionaries: `data/dictionaries/` — plain text files (likely `word frequency` or one word per line depending on loader implementation).
- User-specific data: `data/user_data/` — per-user persisted search counts and custom additions.

To add a new dictionary, place a file in `data/dictionaries/` and confirm the loader in `src/Trie.cpp` picks it up (or add it to the loader list).

## Testing

There are small test programs under `tests/` (for example `tests/test.cpp` and `tests/insert.cpp`). To compile/run a single test quickly:

```bash
g++ -std=c++17 -Iinclude tests/test.cpp -o test_run
./test_run
```

For full test integration, see the `makefile` (it may include targets for `test` or similar).

## Extending & Contributing

- Add new dictionaries to `data/dictionaries/` and update loader logic in `src/Trie.cpp` if necessary.
- To change ranking/boosting, update the merge logic in `src/Trie.cpp` where user history and dictionary frequencies are combined.
- Add unit tests under `tests/` that exercise new behavior.
- Ensure new code compiles with `-std=c++17` and keep third-party header licenses intact in `include/`.

Suggested small improvements (low-risk):

1. Add a `LICENSE` file to the repository.
2. Add a `make test` target (if not already present) that runs the simple tests.
3. Add a small `scripts/` helper to seed `data/` for demos.

## Assumptions & Notes

- I did not run `make` in this session; the `makefile` is present and the original README referred to building with `make`. If your build produces the binary in a different path, update this README accordingly.
- If the server binds to a fixed port, check `src/WebAPI.cpp` for the configured port and any environment or CLI overrides.


