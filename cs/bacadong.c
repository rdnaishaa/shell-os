#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

void print_help() {
    printf("Simple program to read and display the content of a file.\n");
    printf("Usage: bacadong {filename}\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
}

int main(int argc, char *argv[]) {
    int opt;
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // Parsing option
    while ((opt = getopt_long(argc, argv, "h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                return 0;
            default:
                fprintf(stderr, "Usage: bacadong {filename}\n");
                return 1;
        }
    }

    // Pastikan ada 1 argumen lagi setelah options
    if (optind >= argc) {
        fprintf(stderr, "Usage: bacadong {filename}\n");
        return 1;
    }

    char *filename = argv[optind];
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    int ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }
    putchar('\n');
    fclose(file);
    return 0;
}
