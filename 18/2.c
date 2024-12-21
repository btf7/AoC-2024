#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>

#define SIZE 71

struct pos {
    int x;
    int y;
};

void getMap(const char* fileName, struct pos** positions_p, int* positionsCount_p);
void exploreFromTile(bool** map, int** distances, int x, int y);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    struct pos* positions;
    int positionsCount;
    getMap(argv[1], &positions, &positionsCount);

    int result = 0;

    bool** map = malloc(SIZE * sizeof *map);
    for (int i = 0; i < SIZE; i++) map[i] = calloc(SIZE, sizeof **map);

    int** distances = malloc(SIZE * sizeof *distances);
    for (int i = 0; i < SIZE; i++) {
        distances[i] = malloc(SIZE * sizeof **distances);
    }

    for (int i = 0; i < positionsCount; i++) {
        const struct pos pos = positions[i];

        map[pos.y][pos.x] = true;
        assert(!map[0][0]);
        assert(!map[SIZE - 1][SIZE - 1]);

        for (int y = 0; y < SIZE; y++) for (int x = 0; x < SIZE; x++) distances[y][x] = INT_MAX;
        distances[SIZE - 1][SIZE - 1] = 0;

        exploreFromTile(map, distances, SIZE - 1, SIZE - 1);

        if (distances[0][0] == INT_MAX) {
            result = i;
            break;
        }
    }

    assert(result != 0);

    for (int i = 0; i < SIZE; i++) {
        free(map[i]);
        free(distances[i]);
    }
    free(map);
    free(distances);

    printf("%d,%d\n", positions[result].x, positions[result].y);
    free(positions);

    return 0;
}

void getMap(const char* fileName, struct pos** positions_p, int* positionsCount_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c;
    *positionsCount_p = 0;
    int allocated = 1;
    *positions_p = malloc(sizeof **positions_p);

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

        if (*positionsCount_p == allocated) {
            allocated *= 2;
            *positions_p = realloc(*positions_p, allocated * sizeof **positions_p);
        }

        (*positions_p)[*positionsCount_p].x = x;
        (*positions_p)[*positionsCount_p].y = y;
        (*positionsCount_p)++;

        if (c == EOF) break;
        assert(c == '\n');
    }

    fclose(file_p);
    *positions_p = realloc(*positions_p, *positionsCount_p * sizeof **positions_p);
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