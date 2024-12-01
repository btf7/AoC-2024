#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

struct intArray {
    int* arr;
    int len;
};

void getLists(const char* fileName, struct intArray* list1_p, struct intArray* list2_p);
void bubbleSort(struct intArray* arr_p);
int sumDifferences(struct intArray list1, struct intArray list2);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    struct intArray list1 = {NULL, 0};
    struct intArray list2 = {NULL, 0};
    getLists(argv[1], &list1, &list2);
    bubbleSort(&list1);
    bubbleSort(&list2);
    int result = sumDifferences(list1, list2);

    free(list1.arr);
    free(list2.arr);

    printf("%d\n", result);
    return 0;
}

void getLists(const char* fileName, struct intArray* list1_p, struct intArray* list2_p) {
    FILE* file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    char c = getc(file_p);
    char numText[6]; // All of the numbers are 5 digits (or less in test input)
    int len = 0;
    int allocated = 0;

    while (c != EOF) {
        assert(isdigit(c));
        int i = 0;
        do {
            numText[i++] = c;
        } while (isdigit(c = getc(file_p)));
        numText[i] = '\0';

        if (len == allocated) {
            if (allocated) {
                allocated *= 2;
            } else {
                allocated = 1;
            }
            list1_p->arr = realloc(list1_p->arr, allocated * sizeof *list1_p->arr);
            list2_p->arr = realloc(list2_p->arr, allocated * sizeof *list2_p->arr);
        }
        list1_p->arr[len] = atoi(numText);

        assert(c == ' ');
        while ((c = getc(file_p)) == ' ') {}

        assert(isdigit(c));
        i = 0;
        do {
            numText[i++] = c;
        } while (isdigit(c = getc(file_p)));
        numText[i] = '\0';

        list2_p->arr[len++] = atoi(numText);

        assert(c == '\n' || c == EOF);
        c = getc(file_p); // Move onto next line
    }

    fclose(file_p);

    list1_p->arr = realloc(list1_p->arr, len * sizeof *list1_p->arr);
    list2_p->arr = realloc(list2_p->arr, len * sizeof *list2_p->arr);
    list1_p->len = len;
    list2_p->len = len;
}

void bubbleSort(struct intArray* arr_p) {
    for (int i = 1; i < arr_p->len; i++) {
        for (int j = 0; j < arr_p->len - i; j++) {
            const int current = arr_p->arr[j];
            const int next = arr_p->arr[j + 1];
            if (current > next) {
                arr_p->arr[j] = next;
                arr_p->arr[j + 1] = current;
            }
        }
    }
}

int sumDifferences(struct intArray list1, struct intArray list2) {
    int sum = 0;

    for (int i = 0; i < list1.len; i++) {
        const int num1 = list1.arr[i];
        const int num2 = list2.arr[i];
        if (num1 > num2) {
            sum += num1 - num2;
        } else {
            sum += num2 - num1;
        }
    }

    return sum;
}