#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>

#define MAX_LINE 1001
#define ASCII_MIN 32
#define ASCII_MAX 126
#define MAX_LINES 10000
#define MAX_OUT_PER_LINE 8192

typedef struct {
    int ascii;
    int freq;
} CharFreq;

int compare(const void *a, const void *b) {
    CharFreq *x = (CharFreq *)a;
    CharFreq *y = (CharFreq *)b;
    if (x->freq != y->freq)
        return x->freq - y->freq;
    return x->ascii - y->ascii;
}

int main() {
    char *lines[MAX_LINES];
    char *outputs[MAX_LINES];  // Armazena saída de cada linha
    int num_lines = 0;

    // Leitura das linhas
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
        lines[num_lines] = strdup(buffer);
        num_lines++;
    }

    // Alocar memória para as saídas
    for (int i = 0; i < num_lines; i++) {
        outputs[i] = malloc(MAX_OUT_PER_LINE);
        outputs[i][0] = '\0';
    }

    // Processar linhas em paralelo, armazenando saída
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < num_lines; i++) {
        int freq[127] = {0};
        char *line = lines[i];

        for (int j = 0; line[j]; j++) {
            int c = (unsigned char)line[j];
            if (c >= ASCII_MIN && c <= ASCII_MAX)
                freq[c]++;
        }

        CharFreq list[ASCII_MAX - ASCII_MIN + 1];
        int count = 0;
        for (int c = ASCII_MIN; c <= ASCII_MAX; c++) {
            if (freq[c] > 0) {
                list[count].ascii = c;
                list[count].freq = freq[c];
                count++;
            }
        }

        qsort(list, count, sizeof(CharFreq), compare);

        char line_out[MAX_OUT_PER_LINE];
        line_out[0] = '\0';

        for (int j = 0; j < count; j++) {
            char temp[32];
            sprintf(temp, "%d %d\n", list[j].ascii, list[j].freq);
            strcat(line_out, temp);
        }

        strcpy(outputs[i], line_out);
        free(lines[i]);
    }

    // Impressão ordenada
    for (int i = 0; i < num_lines; i++) {
        fputs(outputs[i], stdout);
        if (i < num_lines - 1) printf("\n");  // Só entre blocos
        free(outputs[i]);
    }

    return 0;
}
