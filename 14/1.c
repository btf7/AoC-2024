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

    int topLeft = 0;
    int topRight = 0;
    int bottomLeft = 0;
    int bottomRight = 0;
    assert(WIDTH % 2 == 1);
    assert(HEIGHT % 2 == 1);
    for (int i = 0; i < len; i++) {
        int x = (robots[i].pos.x + robots[i].vel.x * 100) % WIDTH;
        int y = (robots[i].pos.y + robots[i].vel.y * 100) % HEIGHT;
        if (x < 0) x += WIDTH;
        if (y < 0) y += HEIGHT;

        if (x == WIDTH / 2) continue;
        if (y == HEIGHT / 2) continue;

        if (x < WIDTH / 2) {
            if (y < HEIGHT / 2) topLeft++;
            else bottomLeft++;
        } else {
            if (y < HEIGHT / 2) topRight++;
            else bottomRight++;
        }
    }

    free(robots);

    int result = topLeft * topRight * bottomLeft * bottomRight;
    printf("%d\n", result);

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