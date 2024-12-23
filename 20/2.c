#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

struct pos {
    int x;
    int y;
};

void getMap(const char* fileName, bool*** map_p, int* height_p, int* startX_p, int* startY_p);
void getPositions(bool** map, int startX, int startY, struct pos** positions_p, int* positionCount_p);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    bool** map;
    int height, startX, startY;
    getMap(argv[1], &map, &height, &startX, &startY);

    struct pos* positions;
    int positionCount;
    getPositions(map, startX, startY, &positions, &positionCount);

    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);

    int result = 0;
    for (int i = 0; i < positionCount - 1; i++) {
        for (int j = i + 1; j < positionCount; j++) {
            const int steps = abs(positions[i].x - positions[j].x) + abs(positions[i].y - positions[j].y);
            if (steps > 20) continue;
            const int distance = j - i;
            const int saved = distance - steps;
            if(saved >= 100) result++;
        }
    }

    free(positions);

    printf("%d\n", result);
    return 0;
}

void getMap(const char* fileName, bool*** map_p, int* height_p, int* startX_p, int* startY_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c;
    int width = 0;
    int rowsAllocated = 2;
    int boolsAllocated = 1;
    *map_p = malloc(2 * sizeof **map_p);
    **map_p = malloc(sizeof ***map_p);

    // First line
    while ((c = getc(file_p)) != '\n') {
        if (width == boolsAllocated) {
            boolsAllocated *= 2;
            **map_p = realloc(**map_p, boolsAllocated * sizeof ***map_p);
        }

        assert(c == '#');
        (**map_p)[width] = true;

        width++;
    }

    **map_p = realloc(**map_p, width * sizeof ***map_p);

    // Other lines
    *height_p = 1;
    (*map_p)[1] = malloc(width * sizeof ***map_p);
    int x = 0;

    while ((c = getc(file_p)) != EOF) {
        if (c == '\n') {
            (*height_p)++;
            x = 0;

            if (*height_p == rowsAllocated) {
                rowsAllocated *= 2;
                *map_p = realloc(*map_p, rowsAllocated * sizeof **map_p);
            }

            (*map_p)[*height_p] = malloc(width * sizeof ***map_p);
        } else {
            switch (c) {
                case '#':
                (*map_p)[*height_p][x] = true;
                break;

                case 'S':
                *startX_p = x;
                *startY_p = *height_p;
                // Fallthrough
                case 'E':
                case '.':
                (*map_p)[*height_p][x] = false;
                break;

                default:
                assert(false);
            }

            x++;
        }
    }

    fclose(file_p);
    (*height_p)++;
    *map_p = realloc(*map_p, *height_p * sizeof **map_p);
}

void getPositions(bool** map, int startX, int startY, struct pos** positions_p, int* positionCount_p) {
    *positionCount_p = 0;
    int allocated = 1;
    *positions_p = malloc(sizeof **positions_p);

    int x = startX;
    int y = startY;

    while (true) {
        if (*positionCount_p == allocated) {
            allocated *= 2;
            *positions_p = realloc(*positions_p, allocated * sizeof **positions_p);
        }

        assert(!map[y][x]);
        (*positions_p)[*positionCount_p].x = x;
        (*positions_p)[*positionCount_p].y = y;
        (*positionCount_p)++;

        map[y][x] = true; // So we don't go here again

        // There will only be one way to go as it's a track not a maze
        if (!map[y-1][x]) {y--; continue;}
        if (!map[y+1][x]) {y++; continue;}
        if (!map[y][x-1]) {x--; continue;}
        if (!map[y][x+1]) {x++; continue;}
        break;
    }

    *positions_p = realloc(*positions_p, *positionCount_p * sizeof **positions_p);
}