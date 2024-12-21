#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

void getTowels(const char* fileName, char*** towels_p, int* towelCount_p, char*** patterns_p, int* patternCount_p);
size_t getPatternPossibilities(char* pattern, char** towels, int towelCount, char*** patternCache_p, size_t** resultCache_p, int* cacheLen_p, int* cacheCapacity_p);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    char** towels;
    char** patterns;
    int towelCount, patternCount;
    getTowels(argv[1], &towels, &towelCount, &patterns, &patternCount);

    int cacheLen = 0;
    int cacheCapacity = 1;
    char** patternCache = malloc(sizeof *patternCache);
    size_t* resultCache = malloc(sizeof *resultCache);

    size_t result = 0;
    for (int i = 0; i < patternCount; i++) {
        cacheLen = 0; // Reset cache each time as old cache is useless
        result += getPatternPossibilities(patterns[i], towels, towelCount, &patternCache, &resultCache, &cacheLen, &cacheCapacity);
        free(patterns[i]);
    }

    for (int i = 0; i < towelCount; i++) {
        free(towels[i]);
    }
    free(towels);
    free(patterns);
    free(patternCache);
    free(resultCache);

    printf("%lld\n", result);
    return 0;
}

void getTowels(const char* fileName, char*** towels_p, int* towelCount_p, char*** patterns_p, int* patternCount_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c;

    // Towels
    *towelCount_p = 0;
    int towelsAllocated = 1;
    *towels_p = malloc(sizeof **towels_p);

    while (true) {
        if (*towelCount_p == towelsAllocated) {
            towelsAllocated *= 2;
            *towels_p = realloc(*towels_p, towelsAllocated * sizeof **towels_p);
        }

        char** towel_p = &((*towels_p)[(*towelCount_p)++]);
        int len = 0;
        int charsAllocated = 2;
        *towel_p = malloc(2 * sizeof **towel_p);

        while (isalpha(c = getc(file_p))) {
            if (len + 1 == charsAllocated) { // Leave room for '\0'
                charsAllocated *= 2;
                *towel_p = realloc(*towel_p, charsAllocated * sizeof **towel_p);
            }

            (*towel_p)[len++] = c;
        }

        assert(len > 0);
        (*towel_p)[len++] = '\0';
        *towel_p = realloc(*towel_p, len * sizeof **towel_p);

        if (c == '\n') break;
        assert(c == ',');
        c = getc(file_p);
        assert(c == ' ');
    }

    *towels_p = realloc(*towels_p, *towelCount_p * sizeof **towels_p);

    assert(c == '\n');
    c = getc(file_p);
    assert(c == '\n');

    // Patterns
    *patternCount_p = 0;
    int patternsAllocated = 1;
    *patterns_p = malloc(sizeof **patterns_p);

    while (c != EOF) {
        if (*patternCount_p == patternsAllocated) {
            patternsAllocated *= 2;
            *patterns_p = realloc(*patterns_p, patternsAllocated * sizeof **patterns_p);
        }

        char** pattern_p = &((*patterns_p)[(*patternCount_p)++]);
        int len = 0;
        int charsAllocated = 2;
        *pattern_p = malloc(2 * sizeof **pattern_p);

        while (isalpha(c = getc(file_p))) {
            if (len + 1 == charsAllocated) { // Leave room for '\0'
                charsAllocated *= 2;
                *pattern_p = realloc(*pattern_p, charsAllocated * sizeof **pattern_p);
            }

            (*pattern_p)[len++] = c;
        }

        assert(len > 0);
        (*pattern_p)[len++] = '\0';
        *pattern_p = realloc(*pattern_p, len * sizeof **pattern_p);
    }

    fclose(file_p);
    *patterns_p = realloc(*patterns_p, *patternCount_p * sizeof **patterns_p);
}

size_t getPatternPossibilities(char* pattern, char** towels, int towelCount, char*** patternCache_p, size_t** resultCache_p, int* cacheLen_p, int* cacheCapacity_p) {
    if (*pattern == '\0') return true;

    for (int i = 0; i < *cacheLen_p; i++) if (pattern == (*patternCache_p)[i]) return (*resultCache_p)[i];

    size_t result = 0;

    for (int i = 0; i < towelCount; i++) {
        const char* towel = towels[i];
        const size_t len = strlen(towel);
        if (strncmp(pattern, towel, len) != 0) continue;
        result += getPatternPossibilities(pattern + len, towels, towelCount, patternCache_p, resultCache_p, cacheLen_p, cacheCapacity_p);
    }

    if (*cacheLen_p == *cacheCapacity_p) {
        *cacheCapacity_p *= 2;
        *patternCache_p = realloc(*patternCache_p, *cacheCapacity_p * sizeof **patternCache_p);
        *resultCache_p = realloc(*resultCache_p, *cacheCapacity_p * sizeof **resultCache_p);
    }
    (*patternCache_p)[*cacheLen_p] = pattern;
    (*resultCache_p)[*cacheLen_p] = result;
    (*cacheLen_p)++;

    return result;
}