#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

void getMap(const char* fileName, bool*** map_p, int* width_p, int* height_p, int* startX_p, int* startY_p, int* endX_p, int* endY_p);
void exploreDistanceFromTile(bool** map, int** distances, int x, int y, int width, int height);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    bool** map;
    int width, height, startX, startY, endX, endY;
    getMap(argv[1], &map, &width, &height, &startX, &startY, &endX, &endY);

    int** distances = malloc(height * sizeof *distances);
    for (int i = 0; i < height; i++) {
        distances[i] = malloc(width * sizeof **distances);
        for (int j = 0; j < width; j++) {
            distances[i][j] = INT_MAX;
        }
    }
    distances[endY][endX] = 0;
    exploreDistanceFromTile(map, distances, endX, endY, width, height);
    const int tilesToCheck = distances[startY][startX];

    // In 2 steps, can go anywhere in this diamond shape (8 possibilites):
    //   #
    //  # #
    // # S #
    //  # #
    //   #

    int (*saves)[8] = calloc(tilesToCheck, sizeof *saves);
    int x = startX;
    int y = startY;

    for (int i = 0; i < tilesToCheck; i++) {
        if (y-2 >= 0 && !map[y-2][x]) saves[i][0] = distances[y][x] - 2 - distances[y-2][x]; //                        x,   y-2
        if (y+2 < height && !map[y+2][x]) saves[i][1] = distances[y][x] - 2 - distances[y+2][x]; //                    x,   y+2
        if (x-2 >= 0 && !map[y][x-2]) saves[i][2] = distances[y][x] - 2 - distances[y][x-2]; //                        x-2, y
        if (x+2 < width && !map[y][x+2]) saves[i][3] = distances[y][x] - 2 - distances[y][x+2]; //                     x+2, y
        if (y-1 >= 0 && x-1 >= 0 && !map[y-1][x-1]) saves[i][4] = distances[y][x] - 2 - distances[y-1][x-1]; //        x-1, y-1
        if (y+1 < height && x+1 < width && !map[y+1][x+1]) saves[i][5] = distances[y][x] - 2 - distances[y+1][x+1]; // x+1, y+1
        if (y-1 >= 0 && x+1 < width && !map[y-1][x+1]) saves[i][6] = distances[y][x] - 2 - distances[y-1][x+1]; //     x+1, y-1
        if (y+1 < height && x-1 >= 0 && !map[y+1][x-1]) saves[i][7] = distances[y][x] - 2 - distances[y+1][x-1]; //    x-1, y+1

        // Move
        if (distances[y-1][x] < distances[y][x]) y--;
        else if (distances[y+1][x] < distances[y][x]) y++;
        else if (distances[y][x-1] < distances[y][x]) x--;
        else if (distances[y][x+1] < distances[y][x]) x++;
    }

    for (int i = 0; i < height; i++) {
        free(map[i]);
        free(distances[i]);
    }
    free(map);
    free(distances);

    int result = 0;
    for (int i = 0; i < tilesToCheck; i++) for (int j = 0; j < 8; j++) if (saves[i][j] >= 100) result++;

    free(saves);

    printf("%d\n", result);
    return 0;
}

void getMap(const char* fileName, bool*** map_p, int* width_p, int* height_p, int* startX_p, int* startY_p, int* endX_p, int* endY_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    // Copied straight from day 16

    int c;
    *width_p = 0;
    int rowsAllocated = 2;
    int boolsAllocated = 1;
    *map_p = malloc(2 * sizeof **map_p);
    **map_p = malloc(sizeof ***map_p);

    // First line
    while ((c = getc(file_p)) != '\n') {
        if (*width_p == boolsAllocated) {
            boolsAllocated *= 2;
            **map_p = realloc(**map_p, boolsAllocated * sizeof ***map_p);
        }

        assert(c == '#');
        (**map_p)[*width_p] = true;

        (*width_p)++;
    }

    **map_p = realloc(**map_p, *width_p * sizeof ***map_p);

    // Other lines
    *height_p = 1;
    (*map_p)[1] = malloc(*width_p * sizeof ***map_p);
    int x = 0;

    while ((c = getc(file_p)) != EOF) {
        if (c == '\n') {
            (*height_p)++;
            x = 0;

            if (*height_p == rowsAllocated) {
                rowsAllocated *= 2;
                *map_p = realloc(*map_p, rowsAllocated * sizeof **map_p);
            }

            (*map_p)[*height_p] = malloc(*width_p * sizeof ***map_p);
        } else {
            switch (c) {
                case '#':
                (*map_p)[*height_p][x] = true;
                break;

                case '.':
                (*map_p)[*height_p][x] = false;
                break;

                case 'S':
                *startX_p = x;
                *startY_p = *height_p;
                (*map_p)[*height_p][x] = false;
                break;

                case 'E':
                *endX_p = x;
                *endY_p = *height_p;
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

void exploreDistanceFromTile(bool** map, int** distances, int x, int y, int width, int height) {
    // Copied straight from day 18

    assert(distances[y][x] != INT_MAX);
    const int nextDist = distances[y][x] + 1;

    const bool up = y > 0 && !map[y - 1][x] && distances[y - 1][x] > nextDist;
    const bool down = y < height - 1 && !map[y + 1][x] && distances[y + 1][x] > nextDist;
    const bool left = x > 0 && !map[y][x - 1] && distances[y][x - 1] > nextDist;
    const bool right = x < width - 1 && !map[y][x + 1] && distances[y][x + 1] > nextDist;

    if (up) distances[y - 1][x] = nextDist;
    if (down) distances[y + 1][x] = nextDist;
    if (left) distances[y][x - 1] = nextDist;
    if (right) distances[y][x + 1] = nextDist;

    if (up) exploreDistanceFromTile(map, distances, x, y - 1, width, height);
    if (down) exploreDistanceFromTile(map, distances, x, y + 1, width, height);
    if (left) exploreDistanceFromTile(map, distances, x - 1, y, width, height);
    if (right) exploreDistanceFromTile(map, distances, x + 1, y, width, height);
}