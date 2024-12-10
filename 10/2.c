#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

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

// Also gets the rating of any non-0 tile
// Could memoize for better performance but it's actually surprisingly fast
int getRating(const int* map, int x, int y, int width, int height) {
    assert(0 <= x && x < width);
    assert(0 <= y && y < height);

    const int val = map[x + y * width];
    if (val == 9) return 1;

    int rating = 0;
    if (x > 0 && map[x - 1 + y * width] == val + 1) rating += getRating(map, x - 1, y, width, height);
    if (y > 0 && map[x + (y - 1) * width] == val + 1) rating += getRating(map, x, y - 1, width, height);
    if (x < width - 1 && map[x + 1 + y * width] == val + 1) rating += getRating(map, x + 1, y, width, height);
    if (y < height - 1 && map[x + (y + 1) * width] == val + 1) rating += getRating(map, x, y + 1, width, height);
    return rating;
}