#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    FILE* const file_p = fopen(argv[1], "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int result = 0;
    int c = getc(file_p);
    char numText[4]; // All of the numbers are 1 to 3 digits
    bool enabled = true;

    while (c != EOF) {
        if (c == 'm') {

            c = getc(file_p);
            if (!enabled) continue;

            if (c != 'u') continue;
            c = getc(file_p);
            if (c != 'l') continue;
            c = getc(file_p);
            if (c != '(') continue;
            c = getc(file_p);

            if (!isdigit(c)) continue;
            int i = 0;
            do {
                assert(i < 3);
                numText[i++] = c;
            } while (isdigit(c = getc(file_p)));
            numText[i] = '\0';
            const int num1 = atoi(numText);

            if (c != ',') continue;
            c = getc(file_p);

            if (!isdigit(c)) continue;
            i = 0;
            do {
                assert(i < 3);
                numText[i++] = c;
            } while (isdigit(c = getc(file_p)));
            numText[i] = '\0';
            const int num2 = atoi(numText);

            if (c != ')') continue;
            result += num1 * num2;
            c = getc(file_p);

        } else if (c == 'd') {

            c = getc(file_p);
            if (c != 'o') continue;
            c = getc(file_p);

            if (c == '(') {

                c = getc(file_p);
                if (c != ')') continue;
                enabled = true;
                c = getc(file_p);

            } else if (c == 'n') {

            c = getc(file_p);
            if (c != '\'') continue;
            c = getc(file_p);
            if (c != 't') continue;
            c = getc(file_p);
            if (c != '(') continue;
            c = getc(file_p);
            if (c != ')') continue;
            enabled = false;
            c = getc(file_p);

            } else {
                c = getc(file_p);
                continue;
            }

        } else {
            c = getc(file_p);
            continue;
        }
    }

    fclose(file_p);

    printf("%d\n", result);
    return 0;
}