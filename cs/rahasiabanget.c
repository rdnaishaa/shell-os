#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

void encrypt(char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        if ((text[i] >= 'a' && text[i] <= 'z')) {
            text[i] = ((text[i] - 'a' + 3) % 26) + 'a';
        } else if ((text[i] >= 'A' && text[i] <= 'Z')) {
            text[i] = ((text[i] - 'A' + 3) % 26) + 'A';
        }
    }
}

void print_help() {
    printf("Simple Caesar cipher decryption program with a shift of 3 to decode secret messages.\n");
    printf("Usage: rahasiabanget {filename} {content}\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
}

int main(int argc, char *argv[]) {
    int opt;
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "h", long_options, NULL)) != -1) {
        if (opt == 'h') {
            print_help();
            return 0;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Usage: rahasiabanget {filename} {content}\n");
        return 1;
    }

    char *filename = argv[optind];
    char content[1024] = "";

    if (optind + 1 < argc) {
        strncpy(content, argv[optind + 1], sizeof(content) - 1);
        encrypt(content);
    }

    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    fprintf(file, "%s", content);
    fclose(file);

    printf("Suttt diem aja ya file %s rahasia\n", filename);
    return 0;
}
