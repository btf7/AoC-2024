#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

void getInitialMap(const char* fileName, int** map_p, int* len_p);
void compactMap(int** map_p, int* len_p);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    int* map;
    int len;
    getInitialMap(argv[1], &map, &len);
    compactMap(&map, &len);

    size_t result = 0;
    for (int i = 0; i < len; i++) {
        result += i * map[i];
    }
    printf("%lld\n", result);

    free(map);

    return 0;
}

void getInitialMap(const char* fileName, int** map_p, int* len_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c, num;
    int id = 0;
    *len_p = 0;
    int allocated = 1;
    *map_p = malloc(sizeof **map_p);

    while (true) {
        // Number
        c = getc(file_p);
        assert('0' <= c && c <= '9');

        num = c - '0';
        for (int i = 0; i < num; i++) {
            if (*len_p == allocated) {
                allocated *= 2;
                *map_p = realloc(*map_p, allocated * sizeof **map_p);
            }
            (*map_p)[(*len_p)++] = id;
        }
        id++;

        // Blank space
        if ((c = getc(file_p)) == EOF) break;
        assert('0' <= c && c <= '9');

        num = c - '0';
        for (int i = 0; i < num; i++) {
            if (*len_p == allocated) {
                allocated *= 2;
                *map_p = realloc(*map_p, allocated * sizeof **map_p);
            }
            (*map_p)[(*len_p)++] = -1;
        }
    }

    *map_p = realloc(*map_p, *len_p * sizeof **map_p);
}

void compactMap(int** map_p, int* len_p) {
    int p1 = 0;
    int p2 = *len_p - 1;
    while (p1 < p2) {
        if ((*map_p)[p1] != -1) {
            p1++;
        } else if ((*map_p)[p2] == -1) {
            p2--;
            (*len_p)--;
        } else {
            (*map_p)[p1++] = (*map_p)[p2--];
            (*len_p)--;
        }
    }
    // It's shrunk, free extra memory
    *map_p = realloc(*map_p, *len_p * sizeof **map_p);
}