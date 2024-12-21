#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

void getMap(const char* fileName, char** map_p, int* width_p, int* height_p);
void floodFill(const char* map, int width, int height, int i, int* indexes, int* area_p, int* perimeter_p);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    char* map;
    int width, height;
    getMap(argv[1], &map, &width, &height);

    int result = 0;
    for (int i = 0; i < width * height; i++) {
        if (!map[i]) continue;

        int* indexes = malloc(width * height * sizeof *indexes); // This is the maximum it could possibly be - it will never be this big, but this is simpler than accurately sizing it
        int area = 0;
        int perimeter = 0;
        floodFill(map, width, height, i, indexes, &area, &perimeter);

        // Replace all indexes with '\0' so we don't explore them again
        for (int j = 0; j < area; j++) map[indexes[j]] = '\0';

        free(indexes);

        result += area * perimeter;
    }
    printf("%d\n", result);

    free(map);

    return 0;
}

void getMap(const char* fileName, char** map_p, int* width_p, int* height_p) {
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
        assert(isalpha(c));

        if (i == allocated) {
            allocated *= 2;
            *map_p = realloc(*map_p, allocated * sizeof **map_p);
        }

        (*map_p)[i++] = c;
    }
    *width_p = i;

    // Other lines
    *height_p = 2;
    while ((c = getc(file_p)) != EOF) {
        if (c == '\n') {
            (*height_p)++;
            continue;
        }
        assert(isalpha(c));

        if (i == allocated) {
            allocated *= 2;
            *map_p = realloc(*map_p, allocated * sizeof **map_p);
        }

        (*map_p)[i++] = c;
    }

    fclose(file_p);
    *map_p = realloc(*map_p, *width_p * *height_p * sizeof **map_p);
}

void floodFill(const char* map, int width, int height, int i, int* indexes, int* area_p, int* perimeter_p) {
    for (int j = 0; j < *area_p; j++) if (indexes[j] == i) return; // Already been here
    indexes[(*area_p)++] = i;

    const bool topEdge = i < width;
    const bool bottomEdge = i >= width * (height - 1);
    const bool leftEdge = i % width == 0;
    const bool rightEdge = i % width == width - 1;

    if (!topEdge && map[i] == map[i - width]) floodFill(map, width, height, i - width, indexes, area_p, perimeter_p);
    if (!bottomEdge && map[i] == map[i + width]) floodFill(map, width, height, i + width, indexes, area_p, perimeter_p);
    if (!leftEdge && map[i] == map[i - 1]) floodFill(map, width, height, i - 1, indexes, area_p, perimeter_p);
    if (!rightEdge && map[i] == map[i + 1]) floodFill(map, width, height, i + 1, indexes, area_p, perimeter_p);

    if (topEdge || map[i] != map[i - width]) (*perimeter_p)++;
    if (bottomEdge || map[i] != map[i + width]) (*perimeter_p)++;
    if (leftEdge || map[i] != map[i - 1]) (*perimeter_p)++;
    if (rightEdge || map[i] != map[i + 1]) (*perimeter_p)++;
}