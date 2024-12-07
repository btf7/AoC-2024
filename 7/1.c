#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>

struct equation {
    size_t result; // Some of them don't fit in an int
    int* nums;
    int numCount;
};

void getEquations(const char* fileName, struct equation** equations_p, int* equationCount_p);
bool isEquationValid(struct equation equation);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    struct equation* equations;
    int equationCount;
    getEquations(argv[1], &equations, &equationCount);

    size_t result = 0;
    for (int i = 0; i < equationCount; i++) {
        if (isEquationValid(equations[i])) result += equations[i].result;
    }
    printf("%lld\n", result);

    for (int i = 0; i < equationCount; i++) {
        free(equations[i].nums);
    }
    free(equations);

    return 0;
}

void getEquations(const char* fileName, struct equation** equations_p, int* equationCount_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c = getc(file_p);
    *equationCount_p = 1;
    int equationsAllocated = 1;
    *equations_p = malloc(sizeof **equations_p);

    char numText[21]; // size_t max number is 18446744073709551615, which has 20 digits
    struct equation* equation_p = *equations_p;
    equation_p->numCount = 0;
    int numsAllocated = 2; // Should have at least 2
    equation_p->nums = malloc(2 * sizeof *equation_p->nums);

    while (c != EOF) {
        assert(isdigit(c));
        int i = 0;
        do {
            assert(i < 20);
            numText[i++] = c;
        } while (isdigit(c = getc(file_p)));
        numText[i] = '\0';
        equation_p->result = atoll(numText);

        assert(c == ':');

        c = getc(file_p);
        assert(c == ' ');
        while (c == ' ') {
            c = getc(file_p);
            assert(isdigit(c));
            i = 0;
            do {
                assert(i < 20);
                numText[i++] = c;
            } while (isdigit(c = getc(file_p)));
            numText[i] = '\0';

            if (equation_p->numCount == numsAllocated) {
                numsAllocated *= 2;
                equation_p->nums = realloc(equation_p->nums, numsAllocated * sizeof *equation_p->nums);
            }
            equation_p->nums[equation_p->numCount++] = atoi(numText);
        }

        if (c == '\n') {
            equation_p->nums = realloc(equation_p->nums, equation_p->numCount * sizeof *equation_p->nums);

            if (*equationCount_p == equationsAllocated) {
                equationsAllocated *= 2;
                *equations_p = realloc(*equations_p, equationsAllocated * sizeof **equations_p);
            }
            equation_p = &((*equations_p)[(*equationCount_p)++]);
            equation_p->numCount = 0;
            numsAllocated = 2; // Should have at least 2
            equation_p->nums = malloc(2 * sizeof *equation_p->nums);

            c = getc(file_p);
        } else {
            assert(c == EOF);
        }
    }

    *equations_p = realloc(*equations_p, equationsAllocated * sizeof **equations_p);

    fclose(file_p);
}

bool isEquationValid(struct equation equation) {
    assert(equation.numCount > 1);

    for (int operators = 0; operators < pow(2, equation.numCount - 1); operators++) {
        size_t result = equation.nums[0];

        for (int i = 1; i < equation.numCount; i++) {
            if (operators & (1 << (i - 1))) {
                result *= equation.nums[i];
            } else {
                result += equation.nums[i];
            }
        }

        if (result == equation.result) return true;
    }

    return false;
}