#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: rename {oldname} {newname}\n");
        return 1;
    }

    if (rename(argv[1], argv[2]) == 0)
        printf("File renamed successfully.\n");
    else
        perror("Error renaming file");

    return 0;
}
