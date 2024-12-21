#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

struct rule {
    int before;
    int after;
};

struct update {
    int* pages;
    int pageCount;
};

void readFile(const char* fileName, struct rule** rules_p, int* ruleCount_p, struct update** updates_p, int* updateCount_p);
bool checkUpdate(struct update update, const struct rule* rules, int ruleCount);
void fixUpdate(struct update* update_p, const struct rule* rules, int ruleCount);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Expected filename to be given\n");
        exit(1);
    }

    struct rule* rules;
    int ruleCount;
    struct update* updates;
    int updateCount;

    readFile(argv[1], &rules, &ruleCount, &updates, &updateCount);

    int result = 0;
    for (int i = 0; i < updateCount; i++) {
        if (!checkUpdate(updates[i], rules, ruleCount)) {
            fixUpdate(&(updates[i]), rules, ruleCount);
            assert(updates[i].pageCount % 2 == 1);
            result += updates[i].pages[updates[i].pageCount / 2];
        }
    }
    printf("%d\n", result);

    free(rules);
    for (int i = 0; i < updateCount; i++) {
        free(updates[i].pages);
    }
    free(updates);

    return 0;
}

void readFile(const char* fileName, struct rule** rules_p, int* ruleCount_p, struct update** updates_p, int* updateCount_p) {
    FILE* const file_p = fopen(fileName, "r");
    if (!file_p) {
        printf("Failed to open file\n");
        exit(1);
    }

    // Read rules
    *ruleCount_p = 0;
    int rulesAllocated = 1;
    *rules_p = malloc(sizeof **rules_p);
    int c;
    char numText[3] = "00\0"; // Put the \0 in now so I don't have to later

    while (true) {
        c = getc(file_p);

        if (c == '\n') {
            assert(*ruleCount_p > 0);
            *rules_p = realloc(*rules_p, *ruleCount_p * sizeof **rules_p);
            break;
        }

        if ((*ruleCount_p)++ == rulesAllocated) {
            rulesAllocated *= 2;
            *rules_p = realloc(*rules_p, rulesAllocated * sizeof **rules_p);
        }

        assert(isdigit(c));
        numText[0] = c;
        c = getc(file_p);
        assert(isdigit(c));
        numText[1] = c;
        (*rules_p)[*ruleCount_p - 1].before = atoi(numText);

        c = getc(file_p);
        assert(c == '|');

        c = getc(file_p);
        assert(isdigit(c));
        numText[0] = c;
        c = getc(file_p);
        assert(isdigit(c));
        numText[1] = c;
        (*rules_p)[*ruleCount_p - 1].after = atoi(numText);

        c = getc(file_p);
        assert(c == '\n');
    }

    // Read updates
    *updateCount_p = 1;
    int updatesAllocated = 1;
    *updates_p = malloc(sizeof **updates_p);

    struct update* update_p = &((*updates_p)[*updateCount_p - 1]);
    update_p->pageCount = 0;
    int pagesAllocated = 1;
    update_p->pages = malloc(sizeof *update_p->pages);

    while (true) {
        c = getc(file_p);
        assert(isdigit(c));
        numText[0] = c;
        c = getc(file_p);
        assert(isdigit(c));
        numText[1] = c;
        update_p->pages[update_p->pageCount++] = atoi(numText);

        switch (c = getc(file_p)) {
            case ',':
            if (update_p->pageCount == pagesAllocated) {
                pagesAllocated *= 2;
                update_p->pages = realloc(update_p->pages, pagesAllocated * sizeof *update_p->pages);
            }
            break;

            case '\n':
            update_p->pages = realloc(update_p->pages, update_p->pageCount * sizeof *update_p->pages);
            if ((*updateCount_p)++ == updatesAllocated) {
                updatesAllocated *= 2;
                *updates_p = realloc(*updates_p, updatesAllocated * sizeof **updates_p);
            }
            update_p = &((*updates_p)[*updateCount_p - 1]);
            update_p->pageCount = 0;
            pagesAllocated = 1;
            update_p->pages = malloc(sizeof *update_p->pages);
            break;

            case EOF:
            update_p->pages = realloc(update_p->pages, update_p->pageCount * sizeof *update_p->pages);
            *updates_p = realloc(*updates_p, *updateCount_p * sizeof **updates_p);
            return;

            default:
            assert(false);
            break;
        }
    }

    fclose(file_p);
}

bool checkUpdate(struct update update, const struct rule* rules, int ruleCount) {
    for (int i = 1; i < update.pageCount; i++) {
        for (int j = 0; j < ruleCount; j++) {
            if (rules[j].before != update.pages[i]) continue;

            for (int k = 0; k < i; k++) {
                if (rules[j].after == update.pages[k]) return false;
            }
        }
    }

    return true;
}

void fixUpdate(struct update* update_p, const struct rule* rules, int ruleCount) {
    // Same as checkUpdate but swap the pages in the wrong order and try again
    start:
    for (int i = 1; i < update_p->pageCount; i++) {
        for (int j = 0; j < ruleCount; j++) {
            if (rules[j].before != update_p->pages[i]) continue;

            for (int k = 0; k < i; k++) {
                if (rules[j].after == update_p->pages[k]) {
                    const int tmp = update_p->pages[i];
                    update_p->pages[i] = update_p->pages[k];
                    update_p->pages[k] = tmp;
                    goto start; // Ew
                }
            }
        }
    }
}