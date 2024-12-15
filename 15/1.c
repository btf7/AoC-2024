#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

enum tile {WALL, BOX, EMPTY};
enum directions {UP, DOWN, LEFT, RIGHT};

void getMap(const char* fileName, enum tile*** map_p, int* width_p, int* height_p, int* robotX_p, int* robotY_p, enum directions** instructions_p, int* instructionCount_p);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    enum tile** map;
    enum directions* instructions;
    int width, height, robotX, robotY, instructionCount;
    getMap(argv[1], &map, &width, &height, &robotX, &robotY, &instructions, &instructionCount);

    assert(map[robotY][robotX] == EMPTY);

    for (int i = 0; i < instructionCount; i++) {
        switch (instructions[i]) {
            // Silence -Wpedantic
            int x, y;
            bool move;

            case UP:
            y = robotY - 1;
            move = true;
            while (map[y][robotX] != EMPTY) {
                if (map[y][robotX] == WALL) {
                    move = false;
                    break;
                }
                y--;
            }
            if (move) {
                map[y][robotX] = BOX;
                robotY--;
                map[robotY][robotX] = EMPTY;
            }
            break;

            case DOWN:
            y = robotY + 1;
            move = true;
            while (map[y][robotX] != EMPTY) {
                if (map[y][robotX] == WALL) {
                    move = false;
                    break;
                }
                y++;
            }
            if (move) {
                map[y][robotX] = BOX;
                robotY++;
                map[robotY][robotX] = EMPTY;
            }
            break;

            case LEFT:
            x = robotX - 1;
            move = true;
            while (map[robotY][x] != EMPTY) {
                if (map[robotY][x] == WALL) {
                    move = false;
                    break;
                }
                x--;
            }
            if (move) {
                map[robotY][x] = BOX;
                robotX--;
                map[robotY][robotX] = EMPTY;
            }
            break;

            case RIGHT:
            x = robotX + 1;
            move = true;
            while (map[robotY][x] != EMPTY) {
                if (map[robotY][x] == WALL) {
                    move = false;
                    break;
                }
                x++;
            }
            if (move) {
                map[robotY][x] = BOX;
                robotX++;
                map[robotY][robotX] = EMPTY;
            }
            break;
        }
    }

    free(instructions);

    int result = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (map[y][x] != BOX) continue;

            result += 100 * y + x;
        }

        free(map[y]);
    }

    free(map);
    printf("%d\n", result);

    return 0;
}

void getMap(const char* fileName, enum tile*** map_p, int* width_p, int* height_p, int* robotX_p, int* robotY_p, enum directions** instructions_p, int* instructionCount_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c;
    *width_p = 0;
    int rowsAllocated = 2;
    int charsAllocated = 1;
    *map_p = malloc(2 * sizeof **map_p);
    **map_p = malloc(sizeof ***map_p);

    // First line
    while ((c = getc(file_p)) != '\n') {
        if (*width_p == charsAllocated) {
            charsAllocated *= 2;
            **map_p = realloc(**map_p, charsAllocated * sizeof ***map_p);
        }

        switch (c) {
            case '#':
            (**map_p)[*width_p] = WALL;
            break;

            case '@':
            *robotX_p = *width_p;
            *robotY_p = 0;
            // Fallthrough
            case '.':
            (**map_p)[*width_p] = EMPTY;
            break;

            case 'O':
            (**map_p)[*width_p] = BOX;
            break;

            default:
            assert(false);
        }

        (*width_p)++;
    }

    **map_p = realloc(**map_p, *width_p * sizeof ***map_p);

    // Other lines
    *height_p = 1;
    (*map_p)[1] = malloc(*width_p * sizeof ***map_p);
    c = getc(file_p);
    int x = 0;

    while (true) {
        switch (c) {
            case '#':
            (*map_p)[*height_p][x] = WALL;
            break;

            case '@':
            *robotX_p = x;
            *robotY_p = *height_p;
            // Fallthrough
            case '.':
            (*map_p)[*height_p][x] = EMPTY;
            break;

            case 'O':
            (*map_p)[*height_p][x] = BOX;
            break;

            default:
            assert(false);
        }

        x++;

        c = getc(file_p);
        if (c == '\n') {
            (*height_p)++;
            x = 0;

            c = getc(file_p);
            if (c == '\n') break;

            if (*height_p == rowsAllocated) {
                rowsAllocated *= 2;
                *map_p = realloc(*map_p, rowsAllocated * sizeof **map_p);
            }

            (*map_p)[*height_p] = malloc(*width_p * sizeof ***map_p);
        }
    }

    *map_p = realloc(*map_p, *height_p * sizeof **map_p);

    // Instructions
    *instructionCount_p = 0;
    int instructionsAllocated = 1;
    *instructions_p = malloc(sizeof **instructions_p);

    while ((c = getc(file_p)) != EOF) {
        if (c == '\n') continue;

        if (*instructionCount_p == instructionsAllocated) {
            instructionsAllocated *= 2;
            *instructions_p = realloc(*instructions_p, instructionsAllocated * sizeof **instructions_p);
        }

        switch (c) {
            case '^':
            (*instructions_p)[*instructionCount_p] = UP;
            break;

            case 'v':
            (*instructions_p)[*instructionCount_p] = DOWN;
            break;

            case '<':
            (*instructions_p)[*instructionCount_p] = LEFT;
            break;

            case '>':
            (*instructions_p)[*instructionCount_p] = RIGHT;
            break;

            default:
            assert(false);
        }

        (*instructionCount_p)++;
    }

    *instructions_p = realloc(*instructions_p, *instructionCount_p * sizeof **instructions_p);
}