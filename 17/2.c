#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

enum instructions {adv, bxl, bst, jnz, bxc, out, bdv, cdv};

void getInstructions(const char* fileName, enum instructions** instructions_p, int* instructionCount_p);
bool checkSolution(enum instructions* instructions, int instructionCount, size_t A);
size_t readCombo(size_t val, size_t A, size_t B, size_t C);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    int instructionCount;
    enum instructions* instructions;
    getInstructions(argv[1], &instructions, &instructionCount);

    // I know I'm not supposed to share input files, but idk how to make it work for any input
    // 2,4,1,2,7,5,4,3,0,3,1,7,5,5,3,0
    //
    // 0: bst 4   B = A & 7
    // 1: bxl 2   B = B ^ 2
    // 2: cdv 5   C = A >> B
    // 3: bxc 3   B = B ^ C
    // 4: adv 3   A = A >> 3
    // 5: bxl 7   B = B ^ 7
    // 6: out 5   print B & 7
    // 7: jnz 0   restart if A != 0
    //
    // a = !A
    // O = output
    //
    // 0: B = AAA
    // 1: B = AaA
    // 2: C = A >> AaA
    // 3: B = AaA ^ (A >> AaA)
    //
    // 5: B = ...ooo
    // 6: B = ...OOO
    //
    // AaA ^ (A >> AaA) = ...ooo
    //
    // For second character output, repeat with A >> 3
    // Repeat for all 16 characters

    int possibilities[16][8];
    int possibilityCounts[16];
    for (int i = 0; i < 16; i++) possibilityCounts[i]=0;
    size_t A;

    for (A = 0; A < 8; A++) { // First can only have 3 bits
        const int AaA = (A & 7) ^ 2;
        if (((AaA ^ (A >> AaA))&7) != (~instructions[15] & 7)) continue;

        bool alreadyFound = false;
        for (int i = 0; i < possibilityCounts[15]; i++) if (possibilities[15][i] == (int)(A & 7)) alreadyFound = true;
        if (alreadyFound) continue;

        possibilities[15][possibilityCounts[15]++] = A & 7;
    }

    for (A = 0; A < 64; A++) { // Second can only have 6 bits
        const int AaA = (A & 7) ^ 2;
        if (((AaA ^ (A >> AaA))&7) != (~instructions[14] & 7)) continue;

        bool possible = false;
        for (int i = 0; i < possibilityCounts[15]; i++) if (possibilities[15][i] == (int)A >> 3) possible = true;
        if (!possible) continue;

        bool alreadyFound = false;
        for (int i = 0; i < possibilityCounts[14]; i++) if (possibilities[14][i] == (int)(A & 7)) alreadyFound = true;
        if (alreadyFound) continue;

        possibilities[14][possibilityCounts[14]++] = A & 7;
    }

    for (A = 0; A < 512; A++) { // Third can only have 9 bits
        const int AaA = (A & 7) ^ 2;
        if (((AaA ^ (A >> AaA))&7) != (~instructions[13] & 7)) continue;

        bool possible = false;
        for (int i = 0; i < possibilityCounts[15]; i++) if (possibilities[15][i] == (int)A >> 6) possible = true;
        if (!possible) continue;

        possible = false;
        for (int i = 0; i < possibilityCounts[14]; i++) if (possibilities[14][i] == (int)((A >> 3) & 7)) possible = true;
        if (!possible) continue;

        bool alreadyFound = false;
        for (int i = 0; i < possibilityCounts[13]; i++) if (possibilities[13][i] == (int)(A & 7)) alreadyFound = true;
        if (alreadyFound) continue;

        possibilities[13][possibilityCounts[13]++] = A & 7;
    }

    for (int i = 12; i >= 0; i--) {
        for (A = 0; A < 1024; A++) { // Max 10 bits: 3 bits + 7 (0b111) shifts
            const int AaA = (A & 7) ^ 2;
            if (((AaA ^ (A >> AaA))&7) != (~instructions[i] & 7)) continue;

            bool possible = false;
            for (int j = 0; j < possibilityCounts[i + 3]; j++) if ((possibilities[i + 3][j] & 1) == (int)(A >> 9)) possible = true;
            if (!possible) continue;

            possible = false;
            for (int j = 0; j < possibilityCounts[i + 2]; j++) if (possibilities[i + 2][j] == (int)((A >> 6) & 7)) possible = true;
            if (!possible) continue;

            possible = false;
            for (int j = 0; j < possibilityCounts[i + 1]; j++) if (possibilities[i + 1][j] == (int)((A >> 3) & 7)) possible = true;
            if (!possible) continue;

            bool alreadyFound = false;
            for (int j = 0; j < possibilityCounts[i]; j++) if (possibilities[i][j] == (int)(A & 7)) alreadyFound = true;
            if (alreadyFound) continue;

            possibilities[i][possibilityCounts[i]++] = A & 7;
        }
    }

    int indexes[16];
    for (int i = 0; i < 16; i++) indexes[i] = 0;

    while (true) {
        A = 0;

        for (int i = 0; i < 16; i++) {
            A |= (size_t)(possibilities[i][indexes[i]]) << (3 * i);
        }

        if (checkSolution(instructions, instructionCount, A)) printf("%lld\n", A);

        int i = 0;
        bool done = false;
        while (true) {
            if (i == 16) {done = true; break;}
            indexes[i]++;
            if (indexes[i] == possibilityCounts[i]) {
                indexes[i] = 0;
                i++;
                continue;
            }
            i++;
            break;
        }
        if (done) break;
    }

    free(instructions);

    return 0;
}

void getInstructions(const char* fileName, enum instructions** instructions_p, int* instructionCount_p) {
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

    while (isdigit(c = getc(file_p))) {}

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

    while (isdigit(c = getc(file_p))) {}

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

    while (isdigit(c = getc(file_p))) {}

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

        char numText[2];
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

size_t readCombo(size_t val, size_t A, size_t B, size_t C) {
    if (val <= 3) return val;
    if (val == 4) return A;
    if (val == 5) return B;
    if (val == 6) return C;
    assert(false);
}

bool checkSolution(enum instructions* instructions, int instructionCount, size_t A) {
    int outputIndex = 0;
    int PC = 0;
    size_t B = 0;
    size_t C = 0;

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
            if (outputIndex == instructionCount || (readCombo(operand, A, B, C) & 7) != instructions[outputIndex++]) return false;
            break;
        }
    }

    return outputIndex == instructionCount;
}