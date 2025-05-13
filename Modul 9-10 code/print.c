#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

void print_help() {
    printf("Usage: print [OPTION]... [TEXT]\n");
    printf("Options:\n");
    printf("  -u        Print text in uppercase\n");
    printf("  -l        Print text in lowercase\n");
    printf("  -r        Print text in reverse\n");
    printf("  -h        Show this help message\n");
}

void print_uppercase(char *text) {
    for (int i = 0; text[i]; i++) {
        putchar(toupper(text[i]));
    }
    printf("\n");
}

void print_lowercase(char *text) {
    for (int i = 0; text[i]; i++) {
        putchar(tolower(text[i]));
    }
    printf("\n");
}

void print_reverse(char *text) {
    int len = strlen(text);
    for (int i = len - 1; i >= 0; i--) {
        putchar(text[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int opt;
    int upper = 0, lower = 0, reverse = 0;

    while ((opt = getopt(argc, argv, "ulrh")) != -1) {
        switch (opt) {
            case 'u': upper = 1; break;
            case 'l': lower = 1; break;
            case 'r': reverse = 1; break;
            case 'h': print_help(); return 0;
            default:
                fprintf(stderr, "Unknown option. Use -h for help.\n");
                return 1;
        }
    }

    // Ambil input: dari argumen jika ada, kalau tidak, dari stdin (pipe)
    char buffer[1024] = {0};

    if (optind < argc) {
        strcpy(buffer, argv[optind]);
    } else {
        // Baca dari stdin
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            fprintf(stderr, "Error: No input text provided.\n");
            return 1;
        }
        // Hapus newline dari fgets
        buffer[strcspn(buffer, "\n")] = 0;
    }

    printf("ini diprint: ");

    if (upper) {
        print_uppercase(buffer);
    } else if (lower) {
        print_lowercase(buffer);
    } else if (reverse) {
        print_reverse(buffer);
    } else {
        printf("%s\n", buffer);
    }

    return 0;
}
