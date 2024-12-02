#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

struct report {
    char levels[8]; // Can have at most 8, so just allocated space for 8 for simplicity
    unsigned char levelCount;
};

void getReports(const char* fileName, struct report** reports_p, int* reportCount_p);
bool isReportSafe(struct report report);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    struct report* reports = NULL;
    int reportCount = 0;
    getReports(argv[1], &reports, &reportCount);

    int result = 0;
    for (int i = 0; i < reportCount; i++) {
        if (isReportSafe(reports[i])) {
            result++;
        }
    }

    free(reports);

    printf("%d\n", result);
    return 0;
}

void getReports(const char * const fileName, struct report** const reports_p, int* const reportCount_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    int c = getc(file_p);
    char numText[3]; // All of the numbers are 1 or 2 digits
    *reportCount_p = 0;
    int allocated = 0;

    while (c != EOF) {
        if (*reportCount_p == allocated) {
            if (allocated) {
                allocated *= 2;
            } else {
                allocated = 1;
            }
            *reports_p = realloc(*reports_p, allocated * sizeof **reports_p);
        }

        struct report* const report_p = &((*reports_p)[(*reportCount_p)++]);
        report_p->levelCount = 0;

        while (true) {
            assert(isdigit(c));
            assert(report_p->levelCount < 8);

            int i = 0;
            do {
                assert(i < 2);
                numText[i++] = c;
            } while (isdigit(c = getc(file_p)));
            numText[i] = '\0';

            report_p->levels[(report_p->levelCount)++] = atoi(numText);

            assert(c == ' ' || c == '\n' || c == EOF);
            if (c == ' ') {
                c = getc(file_p);
            } else {
                c = getc(file_p);
                break;
            }
        }
    }

    fclose(file_p);

    *reports_p = realloc(*reports_p, *reportCount_p * sizeof **reports_p);
}

bool isReportSafe(const struct report report) {
    assert(report.levelCount > 1);

    const bool increasing = report.levels[0] < report.levels[1];

    for (int i = 0; i < report.levelCount - 1; i++) {
        if (increasing) {
            if (report.levels[i] >= report.levels[i + 1] || report.levels[i + 1] - report.levels[i] > 3) {
                return false;
            }
        } else {
            if (report.levels[i] <= report.levels[i + 1] || report.levels[i] - report.levels[i + 1] > 3) {
                return false;
            }
        }
    }

    return true;
}