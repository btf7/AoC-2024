#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>

#define SIZE 71
#define COUNT 1024

void getMap(const char* fileName, bool** map);
void exploreFromTile(bool** map, int** distances, int x, int y);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    bool** map = malloc(SIZE * sizeof *map);
    for (int i = 0; i < SIZE; i++) map[i] = calloc(SIZE, sizeof **map);
    getMap(argv[1], map);
    assert(!map[0][0]);
    assert(!map[SIZE - 1][SIZE - 1]);

    int** distances = malloc(SIZE * sizeof *distances);
    for (int i = 0; i < SIZE; i++) {
        distances[i] = malloc(SIZE * sizeof **distances);
        for (int j = 0; j < SIZE; j++) distances[i][j] = INT_MAX;
    }
    distances[SIZE - 1][SIZE - 1] = 0;
    exploreFromTile(map, distances, SIZE - 1, SIZE - 1);

    int result = distances[0][0];

    for (int i = 0; i < SIZE; i++) {
        free(map[i]);
        free(distances[i]);
    }
    free(map);
    free(distances);

    printf("%d\n", result);
    return 0;
}

void getMap(const char* fileName, bool** map) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c;
    int count = 0;

    while (true) {
        char numText[3];
        int i = 0;
        while (isdigit(c = getc(file_p))) {
            assert(i < 2);
            numText[i++] = c;
        }
        numText[i] = '\0';
        const int x = atoi(numText);

        assert(c == ',');

        i = 0;
        while (isdigit(c = getc(file_p))) {
            assert(i < 2);
            numText[i++] = c;
        }
        numText[i] = '\0';
        const int y = atoi(numText);

        map[y][x] = true;

        count++;
        if (count == COUNT) break;

        if (c == EOF) break;
        assert(c == '\n');
    }

    fclose(file_p);
}

void exploreFromTile(bool** map, int** distances, int x, int y) {
    assert(distances[y][x] != INT_MAX);
    const int nextDist = distances[y][x] + 1;

    const bool up = y > 0 && !map[y - 1][x] && distances[y - 1][x] > nextDist;
    const bool down = y < SIZE - 1 && !map[y + 1][x] && distances[y + 1][x] > nextDist;
    const bool left = x > 0 && !map[y][x - 1] && distances[y][x - 1] > nextDist;
    const bool right = x < SIZE - 1 && !map[y][x + 1] && distances[y][x + 1] > nextDist;

    if (up) distances[y - 1][x] = nextDist;
    if (down) distances[y + 1][x] = nextDist;
    if (left) distances[y][x - 1] = nextDist;
    if (right) distances[y][x + 1] = nextDist;

    if (up) exploreFromTile(map, distances, x, y - 1);
    if (down) exploreFromTile(map, distances, x, y + 1);
    if (left) exploreFromTile(map, distances, x - 1, y);
    if (right) exploreFromTile(map, distances, x + 1, y);
}