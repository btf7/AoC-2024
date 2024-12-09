#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

struct span {
    int len;
    int id;
};

void getInitialMap(const char* fileName, struct span** map_p, int* len_p);
void compactMap(struct span** map_p, int* len_p);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    struct span* map;
    int len;
    getInitialMap(argv[1], &map, &len);
    compactMap(&map, &len);

    size_t result = 0;
    int i = 0;
    for (int i = 0; i < len; i++) {
        if (map[i].len == 0 || map[i].id == -1) continue;
        for (int j = 0; j < map[i].len; j++) {
            result += (i + j) * map[i].id;
        }
    }
    printf("%lld\n", result);

    free(map);

    return 0;
}

void getInitialMap(const char* fileName, struct span** map_p, int* len_p) {
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

    // Allocate space as if every span was 1 long,
    // but only assign values as appropriate
    // This is weird but so much easier
    while (true) {
        // Number
        c = getc(file_p);
        assert('0' <= c && c <= '9');

        num = c - '0';
        *len_p += num;
        while (*len_p > allocated) {
            allocated *= 2;
            *map_p = realloc(*map_p, allocated * sizeof **map_p);
        }
        (*map_p)[*len_p - num].len = num;
        (*map_p)[*len_p - num].id = id++;
        for (int i = *len_p - num + 1; i < *len_p; i++) {
            (*map_p)[i].len = 0;
        }

        // Blank space
        if ((c = getc(file_p)) == EOF) break;
        assert('0' <= c && c <= '9');

        num = c - '0';
        *len_p += num;
        while (*len_p > allocated) {
            allocated *= 2;
            *map_p = realloc(*map_p, allocated * sizeof **map_p);
        }
        (*map_p)[*len_p - num].len = num;
        (*map_p)[*len_p - num].id = -1;
        for (int i = *len_p - num + 1; i < *len_p; i++) {
            (*map_p)[i].len = 0;
        }
    }

    *map_p = realloc(*map_p, *len_p * sizeof **map_p);
}

void compactMap(struct span** map_p, int* len_p) {
    int p2 = *len_p - 1;

    // Find last id
    while ((*map_p)[p2].len == 0) {
        p2--;
    }

    for (int id = (*map_p)[p2].id; id > 0; id--) {
        // Find p2
        while ((*map_p)[p2].len == 0 || (*map_p)[p2].id != id) {
            p2--;
        }

        // Find first big enough blank space
        int p1 = 0;
        while (!(p1 >= p2 || ((*map_p)[p1].id == -1 && (*map_p)[p1].len >= (*map_p)[p2].len))) {
            p1++;
        }
        if (p1 >= p2) continue;

        // Move it and add new blank space if there is any left
        int space = (*map_p)[p1].len;

        (*map_p)[p1].id = id;
        (*map_p)[p1].len = (*map_p)[p2].len;
        (*map_p)[p2].len = 0;

        if (space > (*map_p)[p1].len) {
            (*map_p)[p1 + (*map_p)[p1].len].len = space - (*map_p)[p1].len;
            (*map_p)[p1 + (*map_p)[p1].len].id = -1;
        }
    }
}