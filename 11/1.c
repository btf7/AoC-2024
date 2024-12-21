#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

struct stone {
    size_t val;
    struct stone* next;
};

struct stone getInitialStones(const char* fileName);
int blink(struct stone* stone_p);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    struct stone firstStone = getInitialStones(argv[1]);

    // Count starting stones
    struct stone* stone_p = &firstStone;
    int result = 1;
    while (stone_p->next) {
        result++;
        stone_p = stone_p->next;
    }

    // Blinks
    for (int i = 0; i < 25; i++) {
        result += blink(&firstStone);
    }
    printf("%d\n", result);

    stone_p = firstStone.next;
    while (stone_p) {
        struct stone* next = stone_p->next;
        free(stone_p);
        stone_p = next;
    }

    return 0;
}

struct stone getInitialStones(const char* fileName) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    char c = getc(file_p);
    char numText[8]; // Biggest number is 7 digits
    struct stone firstStone = {0, NULL};
    struct stone* currentStone_p = &firstStone;

    while (c != EOF) {
        assert(isdigit(c));
        int i = 0;
        do {
            assert(i < 7);
            numText[i++] = c;
        } while (isdigit(c = getc(file_p)));
        numText[i] = '\0';
        currentStone_p->val = atoll(numText);

        assert(c == ' ' || c == EOF);
        if (c == ' ') {
            struct stone* newStone_p = malloc(sizeof *newStone_p);
            currentStone_p->next = newStone_p;
            currentStone_p = newStone_p;
            currentStone_p->next = NULL;
            c = getc(file_p);
        }
    }

    fclose(file_p);

    return firstStone;
}

// Return the number of new stones created
int blink(struct stone* stone_p) {
    int newStones = 0;

    while (stone_p) {
        if (stone_p->val == 0) {
            stone_p->val = 1;
        } else {
            char numText[21]; // size_t max number is 18446744073709551615, which has 20 digits
            snprintf(numText, 21, "%lld", stone_p->val);
            if (strlen(numText) % 2 == 0) {
                const size_t val2 = atoll(numText + strlen(numText) / 2);
                numText[strlen(numText) / 2] = '\0';
                const size_t val1 = atoll(numText);

                stone_p->val = val1;
                struct stone* nextStone = stone_p->next;
                stone_p->next = malloc(sizeof *stone_p->next);
                stone_p = stone_p->next;
                stone_p->val = val2;
                stone_p->next = nextStone;

                newStones++;
            } else {
                stone_p->val *= 2024;
            }
        }

        stone_p = stone_p->next;
    }

    return newStones;
}