#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: hapusfile <filename>\n");
        return 1;
    }

    if (remove(argv[1]) == 0)
        printf("File '%s' deleted successfully!\n", argv[1]);
    else
        perror("Error deleting file");

    return 0;
}
