#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

#define WIDTH 101
#define HEIGHT 103

struct vec2 {
    int x;
    int y;
};

struct robot {
    struct vec2 pos;
    struct vec2 vel;
};

void getRobots(const char* fileName, struct robot** robots_p, int* len_p);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    struct robot* robots;
    int len;
    getRobots(argv[1], &robots, &len);

    // Couldn't think of a nice way to do this,
    // so i just brute force it, and when theres 10+ consecutive robots
    // print it along with i and check if it looks like a tree

    for (int i = 0; i < 10000; i++) {
        // Print
        char img[(WIDTH + 1) * HEIGHT];
        bool print = false;
        int consecutive = 0;
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                bool here = false;
                for (int j = 0; j < len; j++) {
                    if (robots[j].pos.x != x) continue;
                    if (robots[j].pos.y != y) continue;
                    here = true;
                    break;
                }
                if (here) {
                    img[x + y * (WIDTH + 1)] = '#';
                    consecutive++;
                } else {
                    img[x + y * (WIDTH + 1)] = '.';
                    if (consecutive > 9) print = true;
                    consecutive = 0;
                }
            }
            img[y * (WIDTH + 1) + WIDTH] = '\n';
        }
        img[(WIDTH + 1) * HEIGHT - 1] = '\0';
        if (print) printf("%d\n%s", i, img);

        // Move
        for (int j = 0; j < len; j++) {
            robots[j].pos.x = (robots[j].pos.x + robots[j].vel.x) % WIDTH;
            robots[j].pos.y = (robots[j].pos.y + robots[j].vel.y) % HEIGHT;
            if (robots[j].pos.x < 0) robots[j].pos.x += WIDTH;
            if (robots[j].pos.y < 0) robots[j].pos.y += HEIGHT;
        }
    }

    free(robots);

    return 0;
}

void getRobots(const char* fileName, struct robot** robots_p, int* len_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c;
    *len_p = 0;
    int allocated = 1;
    *robots_p = malloc(sizeof **robots_p);
    while (true) {
        if (*len_p == allocated) {
            allocated *= 2;
            *robots_p = realloc(*robots_p, allocated * sizeof **robots_p);
        }

        c = getc(file_p); assert(c == 'p');
        c = getc(file_p); assert(c == '=');

        char numText[4];
        int i = 0;
        while (isdigit(c = getc(file_p))) {
            assert(i < 3);
            numText[i++] = c;
        }
        numText[i] = '\0';
        (*robots_p)[*len_p].pos.x = atoi(numText);

        assert(c == ',');

        i = 0;
        while (isdigit(c = getc(file_p))) {
            assert(i < 3);
            numText[i++] = c;
        }
        numText[i] = '\0';
        (*robots_p)[*len_p].pos.y = atoi(numText);

        assert(c == ' ');
        c = getc(file_p); assert(c == 'v');
        c = getc(file_p); assert(c == '=');

        c = getc(file_p);
        assert(isdigit(c) || c == '-');
        numText[0] = c;
        i = 1;
        while (isdigit(c = getc(file_p))) {
            assert(i < 3);
            numText[i++] = c;
        }
        numText[i] = '\0';
        (*robots_p)[*len_p].vel.x = atoi(numText);

        assert(c == ',');

        c = getc(file_p);
        assert(isdigit(c) || c == '-');
        numText[0] = c;
        i = 1;
        while (isdigit(c = getc(file_p))) {
            assert(i < 3);
            numText[i++] = c;
        }
        numText[i] = '\0';
        (*robots_p)[*len_p].vel.y = atoi(numText);

        (*len_p)++;
        if (c == EOF) break;
        assert(c == '\n');
    }

    *robots_p = realloc(*robots_p, *len_p * sizeof **robots_p);
}