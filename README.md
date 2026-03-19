# utf

A UTF-8 text manipulation library written in C++. Implements byte-level parsing of UTF-8 encoded files and exposes a clean API for searching, inserting, deleting, and replacing multi-byte characters — including characters outside the ASCII range.

## Background

UTF-8 characters range from 1 to 4 bytes depending on the codepoint, which makes standard `char` arrays and string operations unreliable for non-ASCII text. This library solves that by parsing raw bytes into a structured `vector<vector<uint8_t>>` — a vector of characters, where each character is itself a vector of its constituent bytes.

## Features

- **Parsing** — reads a binary file and segments raw bytes into individual UTF-8 characters using bitwise lead-byte detection
- **Search** — finds all occurrences of a substring within the parsed data, returns a vector of indices
- **Insert** — inserts a character sequence at a given index
- **Delete** — removes all occurrences of a given substring
- **Replace** — replaces all occurrences of a substring with a new value; internally tracks index shifts to maintain correctness after each operation
- **Print** — outputs parsed data or search results to stdout

## How UTF-8 parsing works

The lead byte of each character encodes its length via a prefix bit pattern:

| Prefix | Byte length |
|---|---|
| `0xxxxxxx` | 1 byte (ASCII) |
| `110xxxxx` | 2 bytes |
| `1110xxxx` | 3 bytes |
| `11110xxx` | 4 bytes |

The parser applies a bitwise AND mask against each lead byte to determine character width, then reads the appropriate number of continuation bytes and stores them as a single entry in the character vector.

## Usage

```cpp
#include "utf.h"

utf file("input.txt");

// Print the full file
file.print(file.get_data());

// Search for a string
std::vector<size_t> matches = file.search("Never");
file.print(matches);

// Replace all occurrences
file.replace("hello", "world");

// Delete all occurrences
file.delete_value("foo");

// Insert at index
file.insert(5, "bar");
```

## Build

Requires CMake 3.x and a C++17-compatible compiler.

```bash
mkdir build && cd build
cmake ..
make
./utf
```

Tested with GCC and Clang on WSL (Ubuntu).

## Project structure

```
utf/
├── src/        # Library implementation
├── inc/        # Header files
├── exc/        # Exception types
├── main.cpp    # Entry point / usage example
└── CMakeLists.txt
```
