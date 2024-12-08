#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct antenna {
    int x;
    int y;
    char frequency;
};

void getAntennas(const char* fileName, struct antenna** antennas_p, int* antennaCount_p, int* width_p, int* height_p);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    struct antenna* antennas;
    int antennaCount, width, height;
    getAntennas(argv[1], &antennas, &antennaCount, &width, &height);

    bool* antinodes = calloc(width * height, sizeof *antinodes);
    for (int i = 0; i < antennaCount - 1; i++) {
        for (int j = i + 1; j < antennaCount; j++) {
            if (antennas[i].frequency != antennas[j].frequency) continue;

            const int dx = antennas[j].x - antennas[i].x;
            const int dy = antennas[j].y - antennas[i].y;

            int x = antennas[i].x;
            int y = antennas[i].y;
            while (x >= 0 && y >= 0 && x < width && y < height) {
                antinodes[x + y * width] = true;
                x -= dx;
                y -= dy;
            }

            x = antennas[j].x;
            y = antennas[j].y;
            while (x >= 0 && y >= 0 && x < width && y < height) {
                antinodes[x + y * width] = true;
                x += dx;
                y += dy;
            }
        }
    }

    int result = 0;
    for (int i = 0; i < width * height; i++) {
        result += antinodes[i];
    }
    printf("%d\n", result);

    free(antennas);
    free(antinodes);

    return 0;
}

void getAntennas(const char* fileName, struct antenna** antennas_p, int* antennaCount_p, int* width_p, int* height_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c;
    int x = 0;
    int y = 0;
    *antennaCount_p = 0;
    int antennasAllocated = 1;
    *antennas_p = malloc(1 * sizeof **antennas_p);

    while ((c = getc(file_p)) != EOF) {
        if (c == '.') {
            x++;
            continue;
        }
        if (c == '\n') {
            x = 0;
            y++;
            continue;
        }

        if (*antennaCount_p == antennasAllocated) {
            antennasAllocated *= 2;
            *antennas_p = realloc(*antennas_p, antennasAllocated * sizeof **antennas_p);
        }

        (*antennas_p)[*antennaCount_p].x = x;
        (*antennas_p)[*antennaCount_p].y = y;
        (*antennas_p)[*antennaCount_p].frequency = c;
        (*antennaCount_p)++;
        x++;
    }

    fclose(file_p);

    *antennas_p = realloc(*antennas_p, *antennaCount_p * sizeof **antennas_p);

    *width_p = x;
    *height_p = y + 1;
}