#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

struct position {
    size_t x;
    size_t y;
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

    // Puzzle is misleading: there is never multiple solutions. 0 or 1.
    // This took too long to realise :(
    // I've never seen a problem like this before so i made up a weird solution

    // let m = a multiplier, n = b multiplier, p = prize
    // m*a.x + n*b.x = p.x
    // m*a.y + n*b.y = p.y
    // m(a.x + a.y) + n(b.x + b.y) = p.x + p.y
    //
    // n(b.x + b.y) = p.x + p.y - m(a.x + a.y)
    // n = (p.x + p.y - m(a.x + a.y)) / (b.x + b.y)
    //
    // m*a.x + n*b.x = p.x
    // m*a.x + (p.x + p.y - m(a.x + a.y)) / (b.x + b.y) * b.x = p.x
    // m*a.x + (p.x*b.x + p.y*b.x - m*b.x(a.x + a.y)) / (b.x + b.y) = p.x
    // (m*a.x(b.x + b.y) + p.x*b.x + p.y*b.x - m*b.x(a.x + a.y)) / (b.x + b.y) = p.x
    // m*a.x(b.x + b.y) + p.x*b.x + p.y*b.x - m*b.x(a.x + a.y) = p.x(b.x + b.y)
    // m*a.x(b.x + b.y) - m*b.x(a.x + a.y) = p.x(b.x + b.y) - p.x*b.x - p.y*.b.x
    // m(a.x(b.x + b.y) - b.x(a.x + a.y)) = p.x*b.x + p.x*b.y - p.x*b.x - p.y*.b.x
    // m(a.x(b.x + b.y) - b.x(a.x + a.y)) = p.x*b.y - p.y*b.x
    // m = (p.x*b.y - p.y*b.x) / (a.x(b.x + b.y) - b.x(a.x + a.y))
    // m = (p.x*b.y - p.y*b.x) / (a.x*b.x + a.x*b.y - b.x*a.x - b.x*a.y)
    // m = (p.x*b.y - p.y*b.x) / (a.x*b.y - b.x*a.y)
    //
    // m must be an integer, therefore if
    // (p.x*b.y - p.y*b.x) % (a.x*b.y - b.x*a.y) != 0
    // then it is impossible
    //
    // n must be an integer, therefore if
    // (p.x + p.y - m(a.x + a.y)) % (b.x + b.y) != 0
    // then it is impossible

    size_t result = 0;
    for (int i = 0; i < len; i++) {
        const struct machine machine = machines[i];

        long long top = machine.prize.x * machine.b.y - machine.prize.y * machine.b.x;
        long long bottom = machine.a.x * machine.b.y - machine.b.x * machine.a.y;
        assert(bottom != 0);
        if (top % bottom != 0) continue;
        const size_t m = top / bottom;

        top = machine.prize.x + machine.prize.y - m * (machine.a.x + machine.a.y);
        bottom = machine.b.x + machine.b.y;
        if(top % bottom != 0) continue;
        const size_t n = top / bottom;

        assert(m * machine.a.x + n * machine.b.x == machine.prize.x);
        assert(m * machine.a.y + n * machine.b.y == machine.prize.y);
        result += 3 * m + n;
    }

    free(machines);


    printf("%lld\n", result);
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
        (*machines_p)[*len_p].prize.x = atoi(numText) + 10000000000000;

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
        (*machines_p)[*len_p].prize.y = atoi(numText) + 10000000000000;

        (*len_p)++;
        if (c == EOF) break;
        assert(c == '\n');
        c = getc(file_p); assert(c == '\n');
    }

    *machines_p = realloc(*machines_p, *len_p * sizeof **machines_p);
}