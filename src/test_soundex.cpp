// Lightweight unit tests for Soundex library
// No external dependencies - fits project's minimalist philosophy

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PEACOCK_SOUNDEX_IMPLEMENTATION
#include "soundex.h"

// Simple test framework
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void test_##name()
#define RUN_TEST(name) do { \
    printf("  %-40s ", #name); \
    tests_run++; \
    test_##name(); \
} while(0)

#define ASSERT_SOUNDEX(input, expected) do { \
    char result[5] = {0}; \
    Soundex(input, result); \
    if (memcmp(result, expected, 4) == 0) { \
        puts("[PASS]"); \
        tests_passed++; \
    } else { \
        printf("[FAIL] Expected %s, got %.4s\n", expected, result); \
        tests_failed++; \
    } \
} while(0)

// =============================================================================
// Basic functionality tests
// =============================================================================

TEST(basic_name_robert)
{
    ASSERT_SOUNDEX("Robert", "R163");
}

TEST(basic_name_rupert)
{
    // Robert and Rupert should have the same Soundex
    ASSERT_SOUNDEX("Rupert", "R163");
}

TEST(basic_name_smith)
{
    ASSERT_SOUNDEX("Smith", "S530");
}

TEST(basic_name_johnson)
{
    ASSERT_SOUNDEX("Johnson", "J525");
}

TEST(basic_name_williams)
{
    ASSERT_SOUNDEX("Williams", "W452");
}

// =============================================================================
// H and W handling (the bug that was fixed)
// Per spec: letters separated by 'h' or 'w' are coded as a single number
// =============================================================================

TEST(hw_ashcraft)
{
    // A-s(2)-h-c(2)-r(6)-a-f(1)-t(3)
    // The 's' and 'c' are separated by 'h', should be treated as one
    // Result should be A-2-6-1 = A261
    ASSERT_SOUNDEX("Ashcraft", "A261");
}

TEST(hw_ashcroft)
{
    // Same pattern as Ashcraft
    ASSERT_SOUNDEX("Ashcroft", "A261");
}

TEST(hw_tymczak)
{
    // T-y-m(5)-c(2)-z(2)-a-k(2)
    // The c, z, k are all code 2, but separated by nothing/vowel
    // Result: T-5-2-2 = T522
    ASSERT_SOUNDEX("Tymczak", "T522");
}

TEST(hw_pfister)
{
    // P-f(1)-i-s(2)-t(3)-e-r(6)
    // 'p' and 'f' are both code 1, adjacent - keep only first
    // Result: P-2-3-6 = P236
    ASSERT_SOUNDEX("Pfister", "P236");
}

// =============================================================================
// Edge cases
// =============================================================================

TEST(edge_single_letter)
{
    ASSERT_SOUNDEX("A", "A000");
}

TEST(edge_single_letter_l)
{
    ASSERT_SOUNDEX("L", "L000");
}

TEST(edge_all_vowels)
{
    // "Aaaa" - only vowels after first letter
    ASSERT_SOUNDEX("Aaaa", "A000");
}

TEST(edge_short_name_lee)
{
    // L-e-e - only vowels, pad with zeros
    ASSERT_SOUNDEX("Lee", "L000");
}

TEST(edge_adjacent_same_code)
{
    // "Bfpv" - b,f,p,v all map to code 1
    // Since B's code (1) equals f/p/v codes, all are duplicates of first letter
    // Per spec: "This rule also applies to the first letter"
    ASSERT_SOUNDEX("Bfpv", "B000");
}

TEST(edge_vowel_separates_same_code)
{
    // "Bab" - b(1), a(vowel), b(1)
    // Vowel separates them, so both b's should be coded
    ASSERT_SOUNDEX("Bab", "B100");  // Only room for 3 digits, second b would be 4th
}

TEST(edge_case_sensitivity)
{
    // Should handle lowercase
    ASSERT_SOUNDEX("robert", "R163");
    ASSERT_SOUNDEX("ROBERT", "R163");
}

// =============================================================================
// Longer names (test truncation to 3 digits)
// =============================================================================

TEST(long_name_washington)
{
    ASSERT_SOUNDEX("Washington", "W252");
}

TEST(long_name_schwarzenegger)
{
    ASSERT_SOUNDEX("Schwarzenegger", "S625");
}

// =============================================================================
// Test runner
// =============================================================================

int main()
{
    puts("===========================================");
    puts("Soundex Unit Tests");
    puts("===========================================\n");

    puts("Basic functionality:");
    RUN_TEST(basic_name_robert);
    RUN_TEST(basic_name_rupert);
    RUN_TEST(basic_name_smith);
    RUN_TEST(basic_name_johnson);
    RUN_TEST(basic_name_williams);

    puts("\nH and W handling (regression tests for bug fix):");
    RUN_TEST(hw_ashcraft);
    RUN_TEST(hw_ashcroft);
    RUN_TEST(hw_tymczak);
    RUN_TEST(hw_pfister);

    puts("\nEdge cases:");
    RUN_TEST(edge_single_letter);
    RUN_TEST(edge_single_letter_l);
    RUN_TEST(edge_all_vowels);
    RUN_TEST(edge_short_name_lee);
    RUN_TEST(edge_adjacent_same_code);
    RUN_TEST(edge_vowel_separates_same_code);
    RUN_TEST(edge_case_sensitivity);

    puts("\nLong names (truncation):");
    RUN_TEST(long_name_washington);
    RUN_TEST(long_name_schwarzenegger);

    puts("\n===========================================");
    printf("Results: %d/%d passed", tests_passed, tests_run);
    if (tests_failed > 0)
    {
        printf(" (%d FAILED)", tests_failed);
    }
    puts("\n===========================================");

    return tests_failed > 0 ? 1 : 0;
}
