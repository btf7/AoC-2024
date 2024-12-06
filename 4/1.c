#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

void getLines(const char* fileName, char*** lines_p, int* lineCount_p, int* lineLen_p);
int solve(char** lines, int lineCount, int lineLen); // Can't make lines const or compiler complains
bool checkOne(char** lines, int x, int y, int dx, int dy);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    char** lines;
    int lineCount, lineLen;
    getLines(argv[1], &lines, &lineCount, &lineLen);

    int result = solve(lines, lineCount, lineLen);

    printf("%d\n", result);
    return 0;
}

void getLines(const char* fileName, char*** lines_p, int* lineCount_p, int* lineLen_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c;
    *lineCount_p = 1;
    *lineLen_p = 0;
    int linesAllocated = 2;
    int charsAllocated = 1;
    *lines_p = malloc(2 * sizeof **lines_p);
    **lines_p = malloc(1);

    // First line
    while ((c = getc(file_p)) != EOF) {
        if (c == '\n') {
            **lines_p = realloc(**lines_p, *lineLen_p);
            *lineCount_p = 2;
            break;
        }

        if (*lineLen_p == charsAllocated) {
            charsAllocated *= 2;
            **lines_p = realloc(**lines_p, charsAllocated);
        }

        (**lines_p)[(*lineLen_p)++] = c;
    }

    assert(*lineLen_p > 0); // It's not an empty file
    assert(*lineCount_p == 2); // There's more lines to read

    (*lines_p)[*lineCount_p - 1] = malloc(*lineLen_p);

    // Other lines
    int charI = 0;
    while ((c = getc(file_p)) != EOF) {
        if (c == '\n') {
            if ((*lineCount_p)++ == linesAllocated) {
                linesAllocated *= 2;
                *lines_p = realloc(*lines_p, linesAllocated * sizeof **lines_p);
            }
            charI = 0;
            (*lines_p)[*lineCount_p - 1] = malloc(*lineLen_p);
        } else {
            (*lines_p)[*lineCount_p - 1][charI++] = c;
        }
    }

    *lines_p = realloc(*lines_p, *lineCount_p * sizeof **lines_p);

    fclose(file_p);
}

int solve(char** lines, int lineCount, int lineLen) {
    int result = 0;
    bool up, down, left, right;

    for (int y = 0; y < lineCount; y++) {
        up = y >= 3;
        down = y <= lineCount - 4;
        for (int x = 0; x < lineLen; x++) {
            left = x >= 3;
            right = x <= lineLen - 4;

            if (lines[y][x] != 'X') continue;

            if (up) {
                if (left) result += checkOne(lines, x, y, -1, -1);
                result += checkOne(lines, x, y, 0, -1);
                if (right) result += checkOne(lines, x, y, 1, -1);
            }
            if (left) result += checkOne(lines, x, y, -1, 0);
            if (right) result += checkOne(lines, x, y, 1, 0);
            if (down) {
                if (left) result += checkOne(lines, x, y, -1, 1);
                result += checkOne(lines, x, y, 0, 1);
                if (right) result += checkOne(lines, x, y, 1, 1);
            }
        }
    }

    return result;
}

bool checkOne(char** lines, int x, int y, int dx, int dy) {
    x += dx;
    y += dy;
    if (lines[y][x] != 'M') return false;
    x += dx;
    y += dy;
    if (lines[y][x] != 'A') return false;
    x += dx;
    y += dy;
    if (lines[y][x] != 'S') return false;
    return true;
}