#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

// The compiler couldn't find this for some reason??
#define min(a,b) (((a) < (b)) ? (a) : (b))

enum directions {UP, DOWN, LEFT, RIGHT};

struct pos {
    int x;
    int y;
};

struct queue {
    struct pos* tiles;
    int len;
    int capacity;
};

void getMap(const char* fileName, bool*** map_p, int* width_p, int* height_p);
void exploreFromTile(int x, int y, bool** map, unsigned int (**cache)[4], struct queue* queue_p);
void queueAppend(struct queue* queue_p, int x, int y);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

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

    struct queue queue;
    queue.len = 1;
    queue.capacity = 1;
    queue.tiles = malloc(sizeof *queue.tiles);
    queue.tiles[0].x = width - 2;
    queue.tiles[0].y = 1;

    while (queue.len) {
        queue.len--;
        exploreFromTile(queue.tiles[queue.len].x, queue.tiles[queue.len].y, map, cache, &queue);
    }

    int result = min(cache[height - 2][1][RIGHT], cache[height - 2][1][UP] + 1000);

    for (int i = 0; i < height; i++) {
        free(map[i]);
        free(cache[i]);
    }
    free(map);
    free(cache);
    free(queue.tiles);
    printf("%d\n", result);
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

    (*height_p)++;
    *map_p = realloc(*map_p, *height_p * sizeof **map_p);
}

void exploreFromTile(int x, int y, bool** map, unsigned int (**cache)[4], struct queue* queue_p) {
    if (!map[y-1][x]) {
        unsigned int cheapest = UINT_MAX;
        if (cache[y][x][DOWN] != UINT_MAX) cheapest = min(cheapest, cache[y][x][DOWN] + 1);
        if (cache[y][x][LEFT] != UINT_MAX) cheapest = min(cheapest, cache[y][x][LEFT] + 1001);
        if (cache[y][x][RIGHT] != UINT_MAX) cheapest = min(cheapest, cache[y][x][RIGHT] + 1001);

        if (cache[y-1][x][DOWN] > cheapest) {
            cache[y-1][x][DOWN] = cheapest;
            queueAppend(queue_p, x, y-1);
        }
    }
    if (!map[y+1][x]) {
        unsigned int cheapest = UINT_MAX;
        if (cache[y][x][UP] != UINT_MAX) cheapest = min(cheapest, cache[y][x][UP] + 1);
        if (cache[y][x][LEFT] != UINT_MAX) cheapest = min(cheapest, cache[y][x][LEFT] + 1001);
        if (cache[y][x][RIGHT] != UINT_MAX) cheapest = min(cheapest, cache[y][x][RIGHT] + 1001);

        if (cache[y+1][x][UP] > cheapest) {
            cache[y+1][x][UP] = cheapest;
            queueAppend(queue_p, x, y+1);
        }
    }
    if (!map[y][x-1]) {
        unsigned int cheapest = UINT_MAX;
        if (cache[y][x][RIGHT] != UINT_MAX) cheapest = min(cheapest, cache[y][x][RIGHT] + 1);
        if (cache[y][x][UP] != UINT_MAX) cheapest = min(cheapest, cache[y][x][UP] + 1001);
        if (cache[y][x][DOWN] != UINT_MAX) cheapest = min(cheapest, cache[y][x][DOWN] + 1001);

        if (cache[y][x-1][RIGHT] > cheapest) {
            cache[y][x-1][RIGHT] = cheapest;
            queueAppend(queue_p, x-1, y);
        }
    }
    if (!map[y][x+1]) {
        unsigned int cheapest = UINT_MAX;
        if (cache[y][x][LEFT] != UINT_MAX) cheapest = min(cheapest, cache[y][x][LEFT] + 1);
        if (cache[y][x][UP] != UINT_MAX) cheapest = min(cheapest, cache[y][x][UP] + 1001);
        if (cache[y][x][DOWN] != UINT_MAX) cheapest = min(cheapest, cache[y][x][DOWN] + 1001);

        if (cache[y][x+1][LEFT] > cheapest) {
            cache[y][x+1][LEFT] = cheapest;
            queueAppend(queue_p, x+1, y);
        }
    }
}

void queueAppend(struct queue* queue_p, int x, int y) {
    if (queue_p->len == queue_p->capacity) {
        queue_p->capacity *= 2;
        queue_p->tiles = realloc(queue_p->tiles, queue_p->capacity * sizeof *queue_p->tiles);
    }
    queue_p->tiles[queue_p->len].x = x;
    queue_p->tiles[queue_p->len].y = y;
    queue_p->len++;
}