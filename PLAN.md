# Project Completion Plan - Soundex

## Executive Summary

The Soundex project is approximately **75% complete**. The core algorithm is functional, but has a known bug with 'h' and 'w' handling. The permutation utility (`xperm.cpp`) is a stub with no actual implementation. This document outlines what needs to be done to complete the project.

---

## Current State Assessment

### What's Working

| Component | Status | Notes |
|-----------|--------|-------|
| `soundex.h` | Functional | Core algorithm works for most inputs |
| `test.cpp` | Complete | Processes 392K words from wordlist |
| `build.sh` | Complete | Debug/Release builds work |
| `run.sh` | Complete | Test runner works |
| Documentation | Complete | CLAUDE.md is comprehensive |

### What's Incomplete or Broken

| Component | Issue | Priority |
|-----------|-------|----------|
| `soundex.h:66` | Bug: 'h' and 'w' separation rule not implemented | **High** |
| `xperm.cpp` | Stub: `GenerateWordXList()` not implemented | **High** |
| `xperm.cpp:72` | Memory leak: allocated buffer never freed | Medium |
| Testing | No formal unit tests or assertions | Low |

---

## Task Breakdown

### Task 1: Fix the 'h' and 'w' Bug in soundex.h

**Location:** `src/soundex.h:66-79`

**Problem:**
The Soundex specification states that two letters with the same numeric code separated by 'h' or 'w' should be coded as a single number. Currently, when 'h' or 'w' is encountered, `prevValue` is reset to -1, which causes the duplicate detection to fail.

**Example of incorrect behavior:**
- Input: "Ashcraft"
- Current output: A226 (incorrect)
- Expected output: A261 (correct)

**Fix Required:**
```c
// Current (WRONG):
if (value == -1)
{
    prevValue = value;  // Resets to -1, breaking duplicate detection
}

// Fixed:
if (value == -1)
{
    // Do NOT update prevValue - h, w, vowels are transparent
    // for duplicate detection purposes
    continue;
}
```

**Verification:**
- Test with "Ashcraft" → should produce A261
- Test with "Tymczak" → should produce T522
- Test with "Pfister" → should produce P236

---

### Task 2: Implement GenerateWordXList() in xperm.cpp

**Location:** `src/xperm.cpp:53-83`

**Current State:**
The function reads the file into a buffer but immediately returns an empty result without parsing it.

**Required Implementation:**

1. **Parse the buffer** - Split by whitespace to extract individual words (similar to how test.cpp processes words)

2. **Build WordXPair entries** - For each word:
   - Store the word string
   - Compute and store its Soundex code

3. **Populate the WordXList** - Store all pairs in the result structure

4. **Free the temporary buffer** - Fix the memory leak

**Pseudocode:**
```cpp
WordXList GenerateWordXList(const char *path)
{
    WordXList result = {};

    // Open and read file (existing code)
    int fd = open(path, O_RDONLY);
    if (fd == -1) return result;

    struct stat st;
    fstat(fd, &st);
    u64 fileSize = (u64) st.st_size;

    u8 *buffer = (u8*) malloc(fileSize);
    // ... read file ...

    // NEW: Count words first for allocation
    u64 wordCount = CountWords(buffer, fileSize);

    // NEW: Allocate pairs array
    result.pairs = (WordXPair*) malloc(sizeof(WordXPair) * wordCount);
    result.count = wordCount;

    // NEW: Parse words and compute Soundex codes
    u64 idx = 0;
    u64 wordStart = 0;
    for (u64 i = 0; i <= fileSize; i++)
    {
        if (i == fileSize || IsWhitespace(buffer[i]))
        {
            if (i > wordStart)
            {
                // Extract word
                u64 len = i - wordStart;
                result.pairs[idx].word.data = (char*) malloc(len + 1);
                memcpy(result.pairs[idx].word.data, buffer + wordStart, len);
                result.pairs[idx].word.data[len] = '\0';
                result.pairs[idx].word.size = len;

                // Compute Soundex
                Soundex(result.pairs[idx].word.data, result.pairs[idx].soundex);

                idx++;
            }
            wordStart = i + 1;
        }
    }

    free(buffer);  // FIX MEMORY LEAK
    close(fd);
    return result;
}
```

---

### Task 3: Implement Word Matching in xperm.cpp main()

**Location:** `src/xperm.cpp:142-147`

**Current State:**
Just prints the words from `toFind` array without any matching logic.

**Required Implementation:**

1. For each word in `toFind`:
   - Compute its Soundex code
   - Search through `wordXList.pairs` for matches
   - Print all words that share the same Soundex code

**Pseudocode:**
```cpp
for (u64 i = 0; i < toFindSize; i++)
{
    char targetSoundex[4];
    Soundex(toFind[i], targetSoundex);

    printf("Words matching '%s' (Soundex: %.4s):\n", toFind[i], targetSoundex);

    for (u64 j = 0; j < wordXList.count; j++)
    {
        if (memcmp(targetSoundex, wordXList.pairs[j].soundex, 4) == 0)
        {
            printf("  %s\n", wordXList.pairs[j].word.data);
        }
    }
    printf("\n");
}
```

---

### Task 4: Fix Memory Management in xperm.cpp

**Issues:**
1. Buffer allocated in `GenerateWordXList()` is never freed (line 72)
2. `FreeWordXList()` doesn't properly free word strings

**Fix for FreeWordXList():**
```cpp
void FreeWordXList(WordXList *list)
{
    if (list->pairs != NULL)
    {
        for (u64 i = 0; i < list->count; i++)
        {
            free(list->pairs[i].word.data);
        }
        free(list->pairs);
        list->pairs = NULL;
    }
    list->count = 0;
}
```

---

### Task 5 (Optional): Add Unit Tests

**Location:** New file `src/test_soundex.cpp` or add to `test.cpp`

**Test Cases to Add:**

```cpp
// Basic functionality
assert(strcmp(soundex("Robert"), "R163") == 0);
assert(strcmp(soundex("Rupert"), "R163") == 0);

// 'H' and 'W' handling (after bug fix)
assert(strcmp(soundex("Ashcraft"), "A261") == 0);
assert(strcmp(soundex("Tymczak"), "T522") == 0);

// Edge cases
assert(strcmp(soundex("A"), "A000") == 0);
assert(strcmp(soundex("Lee"), "L000") == 0);

// Adjacent duplicates
assert(strcmp(soundex("Pfister"), "P236") == 0);
```

---

## Implementation Order

1. **Fix soundex.h 'h'/'w' bug** - This is a correctness issue that affects all downstream functionality

2. **Implement GenerateWordXList()** - Core functionality needed for xperm to work

3. **Implement word matching in main()** - Complete the xperm utility

4. **Fix memory management** - Prevent memory leaks

5. **(Optional) Add unit tests** - Improve reliability and prevent regressions

---

## Estimated Scope

| Task | Lines of Code | Complexity |
|------|---------------|------------|
| Fix h/w bug | ~5-10 | Low |
| Implement GenerateWordXList | ~50-70 | Medium |
| Implement word matching | ~20-30 | Low |
| Fix memory management | ~10-15 | Low |
| Add unit tests | ~50-100 | Low |

**Total estimated new/modified code:** 135-225 lines

---

## Verification Checklist

After completion, verify:

- [ ] `./build.sh` compiles without warnings
- [ ] `./build.sh release` compiles without warnings
- [ ] `./run.sh Smith` outputs `Smith --> S530`
- [ ] `./run.sh Ashcraft` outputs `Ashcraft --> A261` (h/w bug fixed)
- [ ] `./build/xperm test` finds words matching "test"'s Soundex code
- [ ] No memory leaks (test with valgrind if available)
- [ ] All unit tests pass (if added)

---

## Files to Modify

1. `src/soundex.h` - Fix h/w handling bug
2. `src/xperm.cpp` - Implement GenerateWordXList() and main() matching logic

## Files to Create (Optional)

1. `src/test_soundex.cpp` - Unit tests
