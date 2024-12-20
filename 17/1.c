#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

enum instructions {adv, bxl, bst, jnz, bxc, out, bdv, cdv};

void getInstructions(const char* fileName, enum instructions** instructions_p, int* instructionCount_p, int* A_p, int* B_p, int* C_p);
int readCombo(int val, int A, int B, int C);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    int A, B, C, PC, instructionCount;
    enum instructions* instructions;
    getInstructions(argv[1], &instructions, &instructionCount, &A, &B, &C);

    int outputLen = 0;
    int outputAllocated = 2;
    char* output = malloc(2 * sizeof *output);

    PC = 0;
    while (PC < instructionCount) {
        const enum instructions instruction = instructions[PC];
        const int operand = instructions[PC + 1];
        PC += 2;

        switch (instruction) {
            case adv:
            A = A >> readCombo(operand, A, B, C);
            break;

            case bdv:
            B = A >> readCombo(operand, A, B, C);
            break;

            case cdv:
            C = A >> readCombo(operand, A, B, C);
            break;

            case bxl:
            B = B ^ operand;
            break;

            case bst:
            B = readCombo(operand, A, B, C) & 7;
            break;

            case jnz:
            if (A) PC = operand;
            break;

            case bxc:
            B = B ^ C;
            break;

            case out:
            if (outputLen + 1 >= outputAllocated) {
                outputAllocated *= 2;
                output = realloc(output, outputAllocated * sizeof *output);
            }
            output[outputLen] = (readCombo(operand, A, B, C) & 7) + '0';
            output[outputLen + 1] = ',';
            outputLen += 2;
            break;
        }
    }

    // Cut off the trailing ','
    output[outputLen - 1] = '\0';

    free(instructions);

    printf("%s\n", output);

    free(output);

    return 0;
}

void getInstructions(const char* fileName, enum instructions** instructions_p, int* instructionCount_p, int* A_p, int* B_p, int* C_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c;

    c = getc(file_p); assert(c == 'R');
    c = getc(file_p); assert(c == 'e');
    c = getc(file_p); assert(c == 'g');
    c = getc(file_p); assert(c == 'i');
    c = getc(file_p); assert(c == 's');
    c = getc(file_p); assert(c == 't');
    c = getc(file_p); assert(c == 'e');
    c = getc(file_p); assert(c == 'r');
    c = getc(file_p); assert(c == ' ');
    c = getc(file_p); assert(c == 'A');
    c = getc(file_p); assert(c == ':');
    c = getc(file_p); assert(c == ' ');

    char numText[11]; // int max is 2147483647, 10 digits
    int i = 0;
    while (isdigit(c = getc(file_p))) {
        assert(i < 10);
        numText[i++] = c;
    }
    numText[i] = '\0';
    *A_p = atoi(numText);

    assert(c == '\n');
    c = getc(file_p); assert(c == 'R');
    c = getc(file_p); assert(c == 'e');
    c = getc(file_p); assert(c == 'g');
    c = getc(file_p); assert(c == 'i');
    c = getc(file_p); assert(c == 's');
    c = getc(file_p); assert(c == 't');
    c = getc(file_p); assert(c == 'e');
    c = getc(file_p); assert(c == 'r');
    c = getc(file_p); assert(c == ' ');
    c = getc(file_p); assert(c == 'B');
    c = getc(file_p); assert(c == ':');
    c = getc(file_p); assert(c == ' ');

    i = 0;
    while (isdigit(c = getc(file_p))) {
        assert(i < 10);
        numText[i++] = c;
    }
    numText[i] = '\0';
    *B_p = atoi(numText);

    assert(c == '\n');
    c = getc(file_p); assert(c == 'R');
    c = getc(file_p); assert(c == 'e');
    c = getc(file_p); assert(c == 'g');
    c = getc(file_p); assert(c == 'i');
    c = getc(file_p); assert(c == 's');
    c = getc(file_p); assert(c == 't');
    c = getc(file_p); assert(c == 'e');
    c = getc(file_p); assert(c == 'r');
    c = getc(file_p); assert(c == ' ');
    c = getc(file_p); assert(c == 'C');
    c = getc(file_p); assert(c == ':');
    c = getc(file_p); assert(c == ' ');

    i = 0;
    while (isdigit(c = getc(file_p))) {
        assert(i < 10);
        numText[i++] = c;
    }
    numText[i] = '\0';
    *C_p = atoi(numText);

    assert(c == '\n');
    c = getc(file_p); assert(c == '\n');
    c = getc(file_p); assert(c == 'P');
    c = getc(file_p); assert(c == 'r');
    c = getc(file_p); assert(c == 'o');
    c = getc(file_p); assert(c == 'g');
    c = getc(file_p); assert(c == 'r');
    c = getc(file_p); assert(c == 'a');
    c = getc(file_p); assert(c == 'm');
    c = getc(file_p); assert(c == ':');
    c = getc(file_p); assert(c == ' ');

    *instructionCount_p = 0;
    int allocated = 1;
    *instructions_p = malloc(sizeof **instructions_p);
    while ((c = getc(file_p)) != EOF) {
        assert(isdigit(c));

        if (*instructionCount_p == allocated) {
            allocated *= 2;
            *instructions_p = realloc(*instructions_p, allocated * sizeof **instructions_p);
        }

        numText[0] = c;
        numText[1] = '\0';
        const int num = atoi(numText);
        assert(num >= 0 && num <= 7);
        (*instructions_p)[(*instructionCount_p)++] = num;

        c = getc(file_p);
        assert(c == ',' || c == EOF);
    }

    *instructions_p = realloc(*instructions_p, *instructionCount_p * sizeof **instructions_p);
}

int readCombo(int val, int A, int B, int C) {
    assert(val >= 0);
    if (val <= 3) return val;
    if (val == 4) return A;
    if (val == 5) return B;
    if (val == 6) return C;
    assert(false);
}