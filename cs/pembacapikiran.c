#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

// Caesar decryption with shift 3
void decrypt(char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            text[i] = ((text[i] - 'a' + 26 - 3) % 26) + 'a';
        } else if (text[i] >= 'A' && text[i] <= 'Z') {
            text[i] = ((text[i] - 'A' + 26 - 3) % 26) + 'A';
        }
    }
}

void print_help() {
    printf("Simple program to decrypt a Caesar cipher encoded text with a shift of 3, revealing the original message.\n");
        printf("Usage: pembacapikiran {filename}\n");
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
        fprintf(stderr, "Usage: pembacapikiran {filename}\n");
        return 1;
    }

    char *filename = argv[optind];
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open the file");
        return 1;
    }

    char content[1024] = {0};
    fread(content, 1, sizeof(content) - 1, file);
    fclose(file);

    decrypt(content);
    printf("Rahasianya: %s\n", content);
    return 0;
}
