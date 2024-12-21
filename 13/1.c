#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

struct position {
    int x;
    int y;
};

struct machine {
    struct position a;
    struct position b;
    struct position prize;
};

void getMachines(const char* fileName, struct machine** machines_p, int* len_p);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    struct machine* machines;
    int len;
    getMachines(argv[1], &machines, &len);

    int result = 0;
    for (int i = 0; i < len; i++) {
        const struct machine machine = machines[i];
        int min = 0;

        for (int a = 0; a <= 100; a++) {
            if (machine.a.x * a > machine.prize.x) break;
            if (machine.a.y * a > machine.prize.y) break;

            for (int b = 0; b <= 100; b++) {
                const int x = machine.a.x * a + machine.b.x * b;
                if (x > machine.prize.x) break;
                if (x < machine.prize.x) continue;
                assert(x == machine.prize.x);

                const int y = machine.a.y * a + machine.b.y * b;
                if (y > machine.prize.y) break;
                if (y == machine.prize.y && (min == 0 ||min > 3 * a + b)) min = 3 * a + b;
            }
        }

        if (min != 0) result += min;
    }

    free(machines);


    printf("%d\n", result);
    return 0;
}

void getMachines(const char* fileName, struct machine** machines_p, int* len_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c;
    *len_p = 0;
    int allocated = 1;
    *machines_p = malloc(sizeof **machines_p);
    while (true) {
        if (*len_p == allocated) {
            allocated *= 2;
            *machines_p = realloc(*machines_p, allocated * sizeof **machines_p);
        }

        c = getc(file_p); assert(c == 'B');
        c = getc(file_p); assert(c == 'u');
        c = getc(file_p); assert(c == 't');
        c = getc(file_p); assert(c == 't');
        c = getc(file_p); assert(c == 'o');
        c = getc(file_p); assert(c == 'n');
        c = getc(file_p); assert(c == ' ');
        c = getc(file_p); assert(c == 'A');
        c = getc(file_p); assert(c == ':');
        c = getc(file_p); assert(c == ' ');
        c = getc(file_p); assert(c == 'X');
        c = getc(file_p); assert(c == '+');

        char numText[6];
        c = getc(file_p); assert(isdigit(c));
        numText[0] = c;
        c = getc(file_p); assert(isdigit(c));
        numText[1] = c;
        numText[2] = '\0';
        (*machines_p)[*len_p].a.x = atoi(numText);

        c = getc(file_p); assert(c == ',');
        c = getc(file_p); assert(c == ' ');
        c = getc(file_p); assert(c == 'Y');
        c = getc(file_p); assert(c == '+');

        c = getc(file_p); assert(isdigit(c));
        numText[0] = c;
        c = getc(file_p); assert(isdigit(c));
        numText[1] = c;
        numText[2] = '\0';
        (*machines_p)[*len_p].a.y = atoi(numText);

        c = getc(file_p); assert(c == '\n');
        c = getc(file_p); assert(c == 'B');
        c = getc(file_p); assert(c == 'u');
        c = getc(file_p); assert(c == 't');
        c = getc(file_p); assert(c == 't');
        c = getc(file_p); assert(c == 'o');
        c = getc(file_p); assert(c == 'n');
        c = getc(file_p); assert(c == ' ');
        c = getc(file_p); assert(c == 'B');
        c = getc(file_p); assert(c == ':');
        c = getc(file_p); assert(c == ' ');
        c = getc(file_p); assert(c == 'X');
        c = getc(file_p); assert(c == '+');

        c = getc(file_p); assert(isdigit(c));
        numText[0] = c;
        c = getc(file_p); assert(isdigit(c));
        numText[1] = c;
        numText[2] = '\0';
        (*machines_p)[*len_p].b.x = atoi(numText);

        c = getc(file_p); assert(c == ',');
        c = getc(file_p); assert(c == ' ');
        c = getc(file_p); assert(c == 'Y');
        c = getc(file_p); assert(c == '+');

        c = getc(file_p); assert(isdigit(c));
        numText[0] = c;
        c = getc(file_p); assert(isdigit(c));
        numText[1] = c;
        numText[2] = '\0';
        (*machines_p)[*len_p].b.y = atoi(numText);

        c = getc(file_p); assert(c == '\n');
        c = getc(file_p); assert(c == 'P');
        c = getc(file_p); assert(c == 'r');
        c = getc(file_p); assert(c == 'i');
        c = getc(file_p); assert(c == 'z');
        c = getc(file_p); assert(c == 'e');
        c = getc(file_p); assert(c == ':');
        c = getc(file_p); assert(c == ' ');
        c = getc(file_p); assert(c == 'X');
        c = getc(file_p); assert(c == '=');

        int i = 0;
        while (isdigit(c = getc(file_p))) {
            assert(i < 5);
            numText[i++] = c;
        }
        numText[i] = '\0';
        (*machines_p)[*len_p].prize.x = atoi(numText);

        assert(c == ',');
        c = getc(file_p); assert(c == ' ');
        c = getc(file_p); assert(c == 'Y');
        c = getc(file_p); assert(c == '=');

        i = 0;
        while (isdigit(c = getc(file_p))) {
            assert(i < 5);
            numText[i++] = c;
        }
        numText[i] = '\0';
        (*machines_p)[*len_p].prize.y = atoi(numText);

        (*len_p)++;
        if (c == EOF) break;
        assert(c == '\n');
        c = getc(file_p); assert(c == '\n');
    }

    fclose(file_p);
    *machines_p = realloc(*machines_p, *len_p * sizeof **machines_p);
}