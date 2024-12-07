#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

enum Directions {UP, DOWN, LEFT, RIGHT};

void getMap(const char* fileName, bool*** map_p, int* width_p, int* height_p, int* startX_p, int* startY_p);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    bool** map;
    int width, height, x, y;
    getMap(argv[1], &map, &width, &height, &x, &y);

    bool** visited = malloc(height * sizeof *visited);
    for (int i = 0; i < height; i++) {
        visited[i] = calloc(width, sizeof **visited);
    }
    visited[y][x] = true;

    enum Directions direction = UP;

    while (x >= 0 && y >= 0 && x < width && y < height) {
        switch (direction) {
            case UP:
            if (--y < 0) break;
            if (map[y][x]) {
                direction = RIGHT;
                y++;
                x++;
            }
            visited[y][x] = true;
            break;

            case DOWN:
            if (++y == height) break;
            if (map[y][x]) {
                direction = LEFT;
                y--;
                x--;
            }
            visited[y][x] = true;
            break;

            case LEFT:
            if (--x < 0) break;
            if (map[y][x]) {
                direction = UP;
                y--;
                x++;
            }
            visited[y][x] = true;
            break;

            case RIGHT:
            if (++x == width) break;
            if (map[y][x]) {
                direction = DOWN;
                y++;
                x--;
            }
            visited[y][x] = true;
            break;
        }
    }

    int result = 0;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            result += visited[y][x];
        }
        free(map[y]);
        free(visited[y]);
    }
    free(map);
    free(visited);

    printf("%d\n", result);

    return 0;
}

void getMap(const char* fileName, bool*** map_p, int* width_p, int* height_p, int* startX_p, int* startY_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c;
    *height_p = 1;
    *width_p = 0;
    int rowsAllocated = 2;
    int columnsAllocated = 1;
    *map_p = malloc(2 * sizeof **map_p);
    **map_p = malloc(sizeof ***map_p); // Added sizeof in the hopes that it might optimise bool to be 1 bit

    // First row
    while ((c = getc(file_p)) != EOF) {
        if (c == '\n') {
            **map_p = realloc(**map_p, *width_p * sizeof ***map_p);
            *height_p = 2;
            break;
        }

        if (*width_p == columnsAllocated) {
            columnsAllocated *= 2;
            **map_p = realloc(**map_p, columnsAllocated);
        }

        switch (c) {
            case '#':
            (**map_p)[(*width_p)++] = true;
            break;

            case '^':
            *startX_p = *width_p;
            *startY_p = 0;
            // Fallthrough
            case '.':
            (**map_p)[(*width_p)++] = false;
            break;

            default:
            assert(false);
            break;
        }
    }

    assert(*width_p > 0); // It's not an empty file
    assert(*height_p == 2); // There's more rows to read

    (*map_p)[*height_p - 1] = malloc(*width_p * sizeof ***map_p);

    // Other rows
    int charI = 0;
    while ((c = getc(file_p)) != EOF) {
        switch (c) {
            case '\n':
            if ((*height_p)++ == rowsAllocated) {
                rowsAllocated *= 2;
                *map_p = realloc(*map_p, rowsAllocated * sizeof **map_p);
            }
            charI = 0;
            (*map_p)[*height_p - 1] = malloc(*width_p * sizeof ***map_p);
            break;

            case '#':
            (*map_p)[*height_p - 1][charI++] = true;
            break;

            case '^':
            *startX_p = charI;
            *startY_p = *height_p - 1;
            // Fallthrough
            case '.':
            (*map_p)[*height_p - 1][charI++] = false;
            break;

            default:
            assert(false);
            break;
        }
    }

    *map_p = realloc(*map_p, *height_p * sizeof **map_p);

    fclose(file_p);
}