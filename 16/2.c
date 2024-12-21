#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

// The compiler couldn't find this for some reason??
#define min(a,b) (((a) < (b)) ? (a) : (b))

enum directions {UP, DOWN, LEFT, RIGHT};

struct tile {
    int x;
    int y;
    enum directions dir; // Unused until the end
};

struct posArr {
    struct tile* tiles;
    int len;
    int capacity;
};

void getMap(const char* fileName, bool*** map_p, int* width_p, int* height_p);
void exploreFromTile(int x, int y, bool** map, unsigned int (**cache)[4], struct posArr* stack_p);
void posArrAppend(struct posArr* posArr_p, int x, int y, enum directions dir);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    // Find all the paths

    bool** map;
    int width, height;
    getMap(argv[1], &map, &width, &height);

    unsigned int (**cache)[4];
    cache = malloc(height * sizeof *cache);
    for (int i = 0; i < height; i++) {
        cache[i] = malloc(width * sizeof **cache);
        memset(cache[i], 0xff, width * sizeof **cache);
    }

    cache[1][width - 2][UP] = 0;
    cache[1][width - 2][DOWN] = 0;
    cache[1][width - 2][LEFT] = 0;
    cache[1][width - 2][RIGHT] = 0;

    // Was getting stack overflows (i think) after 16000+ deep recursion, so move it to the heap

    struct posArr stack;
    stack.len = 1;
    stack.capacity = 1;
    stack.tiles = malloc(sizeof *stack.tiles);
    stack.tiles[0].x = width - 2;
    stack.tiles[0].y = 1;

    while (stack.len) {
        stack.len--;
        exploreFromTile(stack.tiles[stack.len].x, stack.tiles[stack.len].y, map, cache, &stack);
    }

    // Count tiles in the shortest path(s)

    struct posArr bestTiles;
    bestTiles.len = 1;
    bestTiles.capacity = 1;
    bestTiles.tiles = malloc(sizeof *bestTiles.tiles);
    bestTiles.tiles[0].x = 1;
    bestTiles.tiles[0].y = height - 2;

    stack.len = 0;
    if (cache[height - 2][1][UP] + 1000 <= cache[height - 2][1][RIGHT]) {
        posArrAppend(&stack, 1, height - 3, UP);
    }
    if (cache[height - 2][1][UP] + 1000 >= cache[height - 2][1][RIGHT]) {
        posArrAppend(&stack, 2, height - 2, RIGHT);
    }

    while (stack.len) {
        stack.len--;
        const struct tile pos = stack.tiles[stack.len];
        assert(!map[pos.y][pos.x]);

        bool skip = false;
        for (int i = 0; i < bestTiles.len; i++) {
            if (pos.x != bestTiles.tiles[i].x) continue;
            if (pos.y != bestTiles.tiles[i].y) continue;
            // We've already been here
            skip = true;
            break;
        }
        if (skip) continue;

        posArrAppend(&bestTiles, pos.x, pos.y, 0);

        if (pos.x == width - 2 && pos.y == 1) continue; // We reached the end

        unsigned int scores[4];
        unsigned int cheapest = UINT_MAX;
        for (int i = 0; i < 4; i++) {
            scores[i] = cache[pos.y][pos.x][i];
            if (scores[i] == UINT_MAX) continue;
            if ((pos.dir == UP || pos.dir == DOWN) && (i == LEFT || i == RIGHT)) scores[i] += 1000;
            if ((pos.dir == LEFT || pos.dir == RIGHT) && (i == UP || i == DOWN)) scores[i] += 1000;
            cheapest = min(cheapest, scores[i]);
        }
        assert(cheapest != UINT_MAX);

        if (scores[UP] == cheapest) posArrAppend(&stack, pos.x, pos.y-1, UP);
        if (scores[DOWN] == cheapest) posArrAppend(&stack, pos.x, pos.y+1, DOWN);
        if (scores[LEFT] == cheapest) posArrAppend(&stack, pos.x-1, pos.y, LEFT);
        if (scores[RIGHT] == cheapest) posArrAppend(&stack, pos.x+1, pos.y, RIGHT);
    }

    for (int i = 0; i < height; i++) {
        free(map[i]);
        free(cache[i]);
    }
    free(map);
    free(cache);
    free(stack.tiles);
    free(bestTiles.tiles);
    printf("%d\n", bestTiles.len);
    return 0;
}

void getMap(const char* fileName, bool*** map_p, int* width_p, int* height_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

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
                case 'S':
                case 'E':
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

void exploreFromTile(int x, int y, bool** map, unsigned int (**cache)[4], struct posArr* stack_p) {
    if (!map[y-1][x]) {
        unsigned int cheapest = UINT_MAX;
        if (cache[y][x][DOWN] != UINT_MAX) cheapest = min(cheapest, cache[y][x][DOWN] + 1);
        if (cache[y][x][LEFT] != UINT_MAX) cheapest = min(cheapest, cache[y][x][LEFT] + 1001);
        if (cache[y][x][RIGHT] != UINT_MAX) cheapest = min(cheapest, cache[y][x][RIGHT] + 1001);

        if (cache[y-1][x][DOWN] > cheapest) {
            cache[y-1][x][DOWN] = cheapest;
            posArrAppend(stack_p, x, y-1, 0);
        }
    }
    if (!map[y+1][x]) {
        unsigned int cheapest = UINT_MAX;
        if (cache[y][x][UP] != UINT_MAX) cheapest = min(cheapest, cache[y][x][UP] + 1);
        if (cache[y][x][LEFT] != UINT_MAX) cheapest = min(cheapest, cache[y][x][LEFT] + 1001);
        if (cache[y][x][RIGHT] != UINT_MAX) cheapest = min(cheapest, cache[y][x][RIGHT] + 1001);

        if (cache[y+1][x][UP] > cheapest) {
            cache[y+1][x][UP] = cheapest;
            posArrAppend(stack_p, x, y+1, 0);
        }
    }
    if (!map[y][x-1]) {
        unsigned int cheapest = UINT_MAX;
        if (cache[y][x][RIGHT] != UINT_MAX) cheapest = min(cheapest, cache[y][x][RIGHT] + 1);
        if (cache[y][x][UP] != UINT_MAX) cheapest = min(cheapest, cache[y][x][UP] + 1001);
        if (cache[y][x][DOWN] != UINT_MAX) cheapest = min(cheapest, cache[y][x][DOWN] + 1001);

        if (cache[y][x-1][RIGHT] > cheapest) {
            cache[y][x-1][RIGHT] = cheapest;
            posArrAppend(stack_p, x-1, y, 0);
        }
    }
    if (!map[y][x+1]) {
        unsigned int cheapest = UINT_MAX;
        if (cache[y][x][LEFT] != UINT_MAX) cheapest = min(cheapest, cache[y][x][LEFT] + 1);
        if (cache[y][x][UP] != UINT_MAX) cheapest = min(cheapest, cache[y][x][UP] + 1001);
        if (cache[y][x][DOWN] != UINT_MAX) cheapest = min(cheapest, cache[y][x][DOWN] + 1001);

        if (cache[y][x+1][LEFT] > cheapest) {
            cache[y][x+1][LEFT] = cheapest;
            posArrAppend(stack_p, x+1, y, 0);
        }
    }
}

void posArrAppend(struct posArr* posArr_p, int x, int y, enum directions dir) {
    if (posArr_p->len == posArr_p->capacity) {
        posArr_p->capacity *= 2;
        posArr_p->tiles = realloc(posArr_p->tiles, posArr_p->capacity * sizeof *posArr_p->tiles);
    }
    posArr_p->tiles[posArr_p->len].x = x;
    posArr_p->tiles[posArr_p->len].y = y;
    posArr_p->tiles[posArr_p->len].dir = dir;
    posArr_p->len++;
}