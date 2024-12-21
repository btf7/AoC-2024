#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

struct stone {
    size_t val;
    size_t count;
};

void getInitialStones(const char* fileName, struct stone** stones_p, int* len_p);
void blink(struct stone** stones_p, int* len_p);
int findStone(const struct stone* stones, int len, size_t val);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    struct stone* stones;
    int len;
    getInitialStones(argv[1], &stones, &len);

    // Blinks
    for (int i = 0; i < 75; i++) {
        blink(&stones, &len);
    }

    size_t result = 0;
    for (int i = 0; i < len; i++) {
        result += stones[i].count;
    }
    printf("%lld\n", result);

    free(stones);

    return 0;
}

void getInitialStones(const char* fileName, struct stone** stones_p, int* len_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    char c = getc(file_p);
    char numText[8]; // Biggest number is 7 digits
    *len_p = 0;
    int allocated = 1;
    *stones_p = malloc(sizeof **stones_p);

    while (c != EOF) {
        assert(isdigit(c));
        int i = 0;
        do {
            assert(i < 7);
            numText[i++] = c;
        } while (isdigit(c = getc(file_p)));
        numText[i] = '\0';

        if (*len_p == allocated) {
            allocated *= 2;
            *stones_p = realloc(*stones_p, allocated * sizeof **stones_p);
        }
        (*stones_p)[*len_p].val = atoll(numText);
        (*stones_p)[(*len_p)++].count = 1;

        assert(c == ' ' || c == EOF);
        c = getc(file_p);
    }

    fclose(file_p);
    *stones_p = realloc(*stones_p, *len_p * sizeof **stones_p);
}

void blink(struct stone** stones_p, int* len_p) {
    const int stonesToBlink = *len_p;
    *len_p = 0;
    int allocated = 1;
    struct stone* newStones = malloc(sizeof *newStones);

    for (int i = 0; i < stonesToBlink; i++) {
        const struct stone stone = (*stones_p)[i];
        if (stone.count == 0) continue;

        if (stone.val == 0) {
            int j = findStone(newStones, *len_p, 1);
            if (j == -1) {
                if (*len_p == allocated) {
                    allocated *= 2;
                    newStones = realloc(newStones, allocated * sizeof *newStones);
                }
                newStones[*len_p].val = 1;
                newStones[(*len_p)++].count = stone.count;
            } else {
                newStones[j].count += stone.count;
            }
        } else {
            char numText[21]; // size_t max number is 18446744073709551615, which has 20 digits
            snprintf(numText, 21, "%lld", stone.val);
            if (strlen(numText) % 2 == 0) {
                const size_t val2 = atoll(numText + strlen(numText) / 2);
                numText[strlen(numText) / 2] = '\0';
                const size_t val1 = atoll(numText);

                int j = findStone(newStones, *len_p, val1);
                if (j == -1) {
                    if (*len_p == allocated) {
                        allocated *= 2;
                        newStones = realloc(newStones, allocated * sizeof *newStones);
                    }
                    newStones[*len_p].val = val1;
                    newStones[(*len_p)++].count = stone.count;
                } else {
                    newStones[j].count += stone.count;
                }

                j = findStone(newStones, *len_p, val2);
                if (j == -1) {
                    if (*len_p == allocated) {
                        allocated *= 2;
                        newStones = realloc(newStones, allocated * sizeof *newStones);
                    }
                    newStones[*len_p].val = val2;
                    newStones[(*len_p)++].count = stone.count;
                } else {
                    newStones[j].count += stone.count;
                }
            } else {
                int j = findStone(newStones, *len_p, stone.val * 2024);
                if (j == -1) {
                    if (*len_p == allocated) {
                        allocated *= 2;
                        newStones = realloc(newStones, allocated * sizeof *newStones);
                    }
                    newStones[*len_p].val = stone.val * 2024;
                    newStones[(*len_p)++].count = stone.count;
                } else {
                    newStones[j].count += stone.count;
                }
            }
        }
    }

    newStones = realloc(newStones, *len_p * sizeof *newStones);
    free(*stones_p);
    *stones_p = newStones;
}

// Returns the index of the stone with the given value, or -1 if there isn't one
int findStone(const struct stone* stones, int len, size_t val) {
    for (int i = 0; i < len; i++) {
        if (stones[i].val == val) return i;
    }
    return -1;
}