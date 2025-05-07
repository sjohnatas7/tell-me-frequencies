#include <stdio.h>
#include <stdlib.h>

struct CharFreq {
    int code, freq;
};

int compare(const void *p1, const void *p2)
{
    struct CharFreq *cf1 = (struct CharFreq *) p1;
    struct CharFreq *cf2 = (struct CharFreq *) p2;

    if (cf1->freq != cf2->freq)
        return cf1->freq - cf2->freq;

    return cf1->code - cf2->code;
}

int main()
{
    char line[1002];
    int first = 1;

    while (fgets(line, 1002, stdin)) {
        struct CharFreq charFreq[96];
        char *ptr;
        int i;

        for (i = 0; i < 96; ++i) {
            charFreq[i].code = i + 32;
            charFreq[i].freq = 0;
        }

        for (ptr = line; *ptr && *ptr != '\n'; ++ptr)
            ++charFreq[*ptr - 32].freq;

        qsort(charFreq, 96, sizeof(struct CharFreq), compare);

        if (!first)
            putchar('\n');

        for (i = 0; i < 96; ++i) {
            if (charFreq[i].freq > 0)
                printf("%d %d\n", charFreq[i].code, charFreq[i].freq);
        }

        first = 0;
    }

    return 0;
}