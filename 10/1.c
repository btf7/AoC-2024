#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

void getMap(const char* fileName, int** map_p, int* width_p, int* height_p);
int getRating(const int* map, int x, int y, int width, int height);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    int* map;
    int width, height;
    getMap(argv[1], &map, &width, &height);

    int result = 0;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (map[x + y * height] == 0) result += getRating(map, x, y, width, height);
        }
    }
    printf("%d\n", result);

    free(map);

    return 0;
}

void getMap(const char* fileName, int** map_p, int* width_p, int* height_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c;
    int i = 0;
    int allocated = 1;
    *map_p = malloc(sizeof **map_p);

    // First line
    while ((c = getc(file_p)) != '\n') {
        assert(isdigit(c));

        if (i == allocated) {
            allocated *= 2;
            *map_p = realloc(*map_p, allocated * sizeof **map_p);
        }

        (*map_p)[i++] = c - '0';
    }
    *width_p = i;

    // Other lines
    *height_p = 2;
    while ((c = getc(file_p)) != EOF) {
        if (c == '\n') {
            (*height_p)++;
            continue;
        }
        assert(isdigit(c));

        if (i == allocated) {
            allocated *= 2;
            *map_p = realloc(*map_p, allocated * sizeof **map_p);
        }

        (*map_p)[i++] = c - '0';
    }

    *map_p = realloc(*map_p, *width_p * *height_p * sizeof **map_p);
}

int getRating(const int* map, int startX, int startY, int width, int height) {
    assert(0 <= startX && startX < width);
    assert(0 <= startY && startY < height);
    assert(map[startX + startY * width] == 0);

    bool* currentPositions = calloc(width * height, sizeof *currentPositions);
    bool* newPositions = calloc(width * height, sizeof *newPositions);
    currentPositions[startX + startY * width] = true;

    for (int val = 1; val <= 9; val++) {
        // Find new possible positions
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                if (!currentPositions[x + y * width]) continue;

                if (x > 0 && map[x - 1 + y * width] == val) newPositions[x - 1 + y * width] = true;
                if (y > 0 && map[x + (y - 1) * width] == val) newPositions[x + (y - 1) * width] = true;
                if (x < width - 1 && map[x + 1 + y * width] == val) newPositions[x + 1 + y * width] = true;
                if (y < height - 1 && map[x + (y + 1) * width] == val) newPositions[x + (y + 1) * width] = true;
            }
        }

        // Make new positions the current positions
        bool* tmp = newPositions;
        newPositions = currentPositions;
        currentPositions = tmp;

        // Reset newPositions
        memset(newPositions, false, width * height * sizeof *newPositions);
    }
    free(newPositions);

    int score = 0;
    for (int i = 0; i < width * height; i++) {
        if (currentPositions[i]) score++;
    }
    free(currentPositions);
    return score;
}