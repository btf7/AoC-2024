#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdint.h>

// Numbers are between 1 and 99999, therefore uint32_t for numbers
// There are 6 numbers in the test input, 1000 in the real input, therefore uint16_t for indexes

void getLists(const char* fileName, uint32_t** list1_p, uint32_t** list2_p, uint16_t* len_p);
void bubbleSort(uint32_t* arr, uint16_t len);
size_t sumDifferences(const uint32_t* list1, const uint32_t* list2, uint16_t len);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    uint32_t* list1 = NULL;
    uint32_t* list2 = NULL;
    uint16_t len;
    getLists(argv[1], &list1, &list2, &len);
    bubbleSort(list1, len);
    bubbleSort(list2, len);
    const size_t result = sumDifferences(list1, list2, len);

    free(list1);
    free(list2);

    printf("%lld\n", result);
    return 0;
}

void getLists(const char* fileName, uint32_t** const list1_p, uint32_t** const list2_p, uint16_t* const len_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c = getc(file_p);
    char numText[6]; // All of the numbers are 1 or 5 digits
    uint16_t allocated = 0;
    *len_p = 0;

    while (c != EOF) {
        assert(isdigit(c));
        uint8_t i = 0;
        do {
            assert(i < 5);
            numText[i++] = c;
        } while (isdigit(c = getc(file_p)));
        numText[i] = '\0';

        if (*len_p == allocated) {
            if (allocated) {
                allocated *= 2;
            } else {
                allocated = 1;
            }
            *list1_p = realloc(*list1_p, allocated * sizeof **list1_p);
            *list2_p = realloc(*list2_p, allocated * sizeof **list2_p);
        }
        (*list1_p)[*len_p] = atoi(numText);

        assert(c == ' ');
        while ((c = getc(file_p)) == ' ') {}

        assert(isdigit(c));
        i = 0;
        do {
            assert(i < 5);
            numText[i++] = c;
        } while (isdigit(c = getc(file_p)));
        numText[i] = '\0';

        (*list2_p)[(*len_p)++] = atoi(numText);

        assert(c == '\n' || c == EOF);
        c = getc(file_p); // Move onto the next line
    }

    fclose(file_p);

    *list1_p = realloc(*list1_p, *len_p * sizeof **list1_p);
    *list2_p = realloc(*list2_p, *len_p * sizeof **list2_p);
}

void bubbleSort(uint32_t* const arr, const uint16_t len) {
    for (uint16_t i = 1; i < len; i++) {
        for (uint16_t j = 0; j < len - i; j++) {
            const uint32_t current = arr[j];
            const uint32_t next = arr[j + 1];
            if (current > next) {
                arr[j] = next;
                arr[j + 1] = current;
            }
        }
    }
}

size_t sumDifferences(const uint32_t * const list1, const uint32_t * const list2, const uint16_t len) {
    size_t sum = 0;

    for (uint16_t i = 0; i < len; i++) {
        const uint32_t num1 = list1[i];
        const uint32_t num2 = list2[i];
        if (num1 > num2) {
            sum += num1 - num2;
        } else {
            sum += num2 - num1;
        }
    }

    return sum;
}