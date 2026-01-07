# CLAUDE.md - AI Assistant Guide for Soundex

## Project Overview

**Soundex** is a C library implementing the Soundex phonetic algorithm, which converts names/words into phonetic codes for matching names that sound similar despite different spellings. The output is a 4-character code: one letter followed by three digits.

**Author**: Garrison Peacock
**License**: MIT (2019)

## Codebase Structure

```
/home/user/Soundex/
├── src/
│   ├── soundex.h       # Core Soundex algorithm (header-only STB-style library)
│   ├── test.cpp        # Main test program
│   └── xperm.cpp       # Permutation utility (INCOMPLETE/WIP)
├── build.sh            # Build script (clang++)
├── run.sh              # Run script
├── words.txt           # Test wordlist (392K words)
├── out.txt             # Generated test output
└── LICENSE             # MIT License
```

## Build System

### Commands

```bash
# Debug build (default)
./build.sh

# Release build
./build.sh release

# Run the test program
./run.sh [arguments]
```

### Build Configuration

- **Compiler**: `clang++`
- **Flags**: `-Wall -Wextra -fno-exceptions -fno-rtti`
- **Debug**: `-O0 -g -DDEBUG`
- **Release**: `-O3 -flto`
- **Output**: `build/soundex_test`, `build/xperm`

## Key Source Files

### soundex.h (Core Library)

STB-style single-header library. Usage:

```c
// In ONE .c/.cpp file:
#define PEACOCK_SOUNDEX_IMPLEMENTATION
#include "soundex.h"

// In other files, just include:
#include "soundex.h"
```

**Main Function**:
```c
int Soundex(const char *input, char *out);
// Returns: number of bytes written (always 4)
// out: must be at least 4 bytes, receives the Soundex code
// Note: No error checking is performed
```

### test.cpp

Test program that:
1. Reads `words.txt` and computes Soundex codes for all words
2. Processes command-line arguments through Soundex

### xperm.cpp (INCOMPLETE)

Work-in-progress utility for finding word permutations. The `GenerateWordXList()` function is not implemented.

## Code Conventions

### Naming

- **Functions**: PascalCase (`Soundex`, `IsWhitespace`, `GenerateWordXList`)
- **Variables**: camelCase (`fileSize`, `prevValue`)
- **Constants**: UPPER_CASE (`CHAR_LOOKUP`)
- **Type aliases**: lowercase (`u8`, `u32`, `s64`)

### Type Definitions

```cpp
typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
```

### Style Guidelines

- Use POSIX system calls (`open`, `read`, `fstat`) over C++ streams
- Manual memory management with `malloc`/`free`
- Minimal C++ features (exceptions/RTTI disabled)
- C-compatible code with `extern "C"` blocks where needed

## Testing

No formal test framework. Testing is done via:

1. **File test**: Automatically processes `words.txt` on startup
2. **Argument test**: Pass words as command-line arguments

```bash
./build/soundex_test Smith Johnson Williams
```

Output format: `word --> soundex_code`

## Dependencies

**None** - Pure C with POSIX system calls only.

Required headers:
- `<sys/types.h>`, `<sys/stat.h>`, `<fcntl.h>`, `<unistd.h>`
- `<stdio.h>`, `<stdint.h>`, `<stdlib.h>`

## Platform Support

- Linux (primary)
- Windows (win32 compatibility addressed)

## Known Issues & TODOs

1. **soundex.h:66** - The 'h' and 'w' separation rule is not fully implemented:
   ```
   TODO(Peacock): This is incorrect, see step 3 above concerning 'h' and 'w'.
   ```
   Per the Soundex algorithm, letters separated by 'h' or 'w' should be treated as adjacent for duplicate removal, but this is not yet handled.

2. **xperm.cpp** - The `GenerateWordXList()` function is a stub that returns an empty result.

## Algorithm Reference

The Soundex algorithm implemented:
1. Retain the first letter of the name
2. Drop vowels (a, e, i, o, u) and h, w, y
3. Replace consonants with digits:
   - b, f, p, v → 1
   - c, g, j, k, q, s, x, z → 2
   - d, t → 3
   - l → 4
   - m, n → 5
   - r → 6
4. Remove adjacent duplicate digits
5. Pad with zeros to get exactly 3 digits

## Development Notes

- Commit messages use imperative mood ("Fix", "Add", "Update")
- The project uses a simple build script rather than Make/CMake
- All builds output to the `build/` directory
- Data files (`words.txt`, `out.txt`) are copied to build directory
