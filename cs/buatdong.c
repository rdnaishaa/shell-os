#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Fungsi untuk menampilkan bantuan
void print_help() {
    printf("Simple program to write content to a file. Content can be written in uppercase or lowercase.\n");
    printf("Usage: buatdong {filename} {content}\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
}

int main(int argc, char *argv[]) {
    // Cek jika hanya flag -h atau --help yang diberikan
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_help();
        return 0;
    }

    // Cek jumlah argumen
    if (argc != 3) {
        fprintf(stderr, "Usage: buatdong {filename} {content}\n");
        return 1;
    }

    char *filename = argv[1];
    char *content = argv[2];

    // Buka file untuk menulis
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Tulis isi konten ke file
    fprintf(file, "%s", content);
    fclose(file);

    printf("Written to %s\n", filename);
    return 0;
}
