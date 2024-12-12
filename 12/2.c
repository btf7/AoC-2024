#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

void getMap(const char* fileName, char** map_p, int* width_p, int* height_p);
void floodFill(const char* map, int width, int height, int i, bool* region, int* area_p);
int getPerimeter(const bool* region, int width, int height);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    char* map;
    int width, height;
    getMap(argv[1], &map, &width, &height);

    bool* region = malloc(width * height * sizeof *region);
    int result = 0;
    for (int i = 0; i < width * height; i++) {
        if (!map[i]) continue;

        int area = 0;
        memset(region, false, width * height);
        floodFill(map, width, height, i, region, &area);

        int perimeter = getPerimeter(region, width, height);

        // Replace region with '\0' so we don't explore it again
        for (int j = 0; j < width * height; j++) if (region[j]) map[j] = '\0';

        result += area * perimeter;
    }
    printf("%d\n", result);

    free(map);
    free(region);

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

    *map_p = realloc(*map_p, *width_p * *height_p * sizeof **map_p);
}

void floodFill(const char* map, int width, int height, int i, bool* region, int* area_p) {
    if (region[i]) return; // Already been here
    region[i] = true;
    (*area_p)++;

    const bool topEdge = i < width;
    const bool bottomEdge = i >= width * (height - 1);
    const bool leftEdge = i % width == 0;
    const bool rightEdge = i % width == width - 1;

    if (!topEdge && map[i] == map[i - width]) floodFill(map, width, height, i - width, region, area_p);
    if (!bottomEdge && map[i] == map[i + width]) floodFill(map, width, height, i + width, region, area_p);
    if (!leftEdge && map[i] == map[i - 1]) floodFill(map, width, height, i - 1, region, area_p);
    if (!rightEdge && map[i] == map[i + 1]) floodFill(map, width, height, i + 1, region, area_p);
}

int getPerimeter(const bool* region, int width, int height) {
    int perimeter = 0;

    bool* topEdges = calloc(width * height, sizeof *topEdges);
    bool* bottomEdges = calloc(width * height, sizeof *bottomEdges);
    bool* leftEdges = calloc(width * height, sizeof *leftEdges);
    bool* rightEdges = calloc(width * height, sizeof *rightEdges);

    for (int i = 0; i < width * height; i++) {
        if (!region[i]) continue;

        const bool topEdge = i < width;
        const bool bottomEdge = i >= width * (height - 1);
        const bool leftEdge = i % width == 0;
        const bool rightEdge = i % width == width - 1;

        // Since we check tiles top to bottom left to right, we can skip some directions

        if (!topEdges[i] && (topEdge || !region[i - width])) {
            topEdges[i] = true;
            perimeter++;

            // Skip left, go right
            int j = i;
            while (true) {
                j++;
                if (j % width == 0) break; // If we wrapped to the left edge
                if (!region[j]) break; // If the edge has ended                                                  ^
                if (!topEdge && region[j - width]) break; // If the edge has turned up (become a left edge) eg. >^
                topEdges[j] = true;
            }
        }

        if (!bottomEdges[i] && (bottomEdge || !region[i + width])) {
            bottomEdges[i] = true;
            perimeter++;

            // Skip left, go right
            int j = i;
            while (true) {
                j++;
                if (j % width == 0) break; // If we wrapped to the left edge
                if (!region[j]) break; // If the edge has ended                                                      >v
                if (!bottomEdge && region[j + width]) break; // If the edge has turned down (become a left edge) eg.  v
                bottomEdges[j] = true;
            }
        }

        if (!leftEdges[i] && (leftEdge || !region[i - 1])) {
            leftEdges[i] = true;
            perimeter++;

            // Skip up, go down
            int j = i;
            while (true) {
                j += width;
                if (j >= width * height) break; // If we went off the bottom
                if (!region[j]) break; // If the edge has ended                                                v
                if (!leftEdge && region[j - 1]) break; // If the edge has turned left (become a top edge) eg. <<
                leftEdges[j] = true;
            }
        }

        if (!rightEdges[i] && (rightEdge || !region[i + 1])) {
            rightEdges[i] = true;
            perimeter++;

            // Skip up, go down
            int j = i;
            while (true) {
                j += width;
                if (j >= width * height) break; // If we went off the bottom
                if (!region[j]) break; // If the edge has ended                                                 v
                if (!rightEdge && region[j + 1]) break; // If the edge has turned right (become a top edge) eg. >>
                rightEdges[j] = true;
            }
        }
    }

    free(topEdges);
    free(bottomEdges);
    free(leftEdges);
    free(rightEdges);

    return perimeter;
}