#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>

// Function prototypes
void search_in_file(const char *file_path, const char *pattern, int case_insensitive, int show_line_numbers, int count_only);
void search_in_directory(const char *dir_path, const char *pattern, int recursive, int case_insensitive, int show_line_numbers, int count_only);
void print_help();
int is_regular_file(const char *path);
int line_contains_pattern(const char *line, const char *pattern, int case_insensitive);

int main(int argc, char *argv[]) {
    // If no arguments or help flag, print help and exit
    if (argc == 1 || (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))) {
        print_help();
        return 0;
    }

    // Default values for flags
    int recursive = 0;
    int case_insensitive = 0;
    int show_line_numbers = 0;
    int count_only = 0;
    char *pattern = NULL;
    char *directory = "."; // Default to current directory

    // Parse flags
    int i;
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // Process flag
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'r':
                        recursive = 1;
                        break;
                    case 'i':
                        case_insensitive = 1;
                        break;
                    case 'n':
                        show_line_numbers = 1;
                        break;
                    case 'c':
                        count_only = 1;
                        break;
                    default:
                        fprintf(stderr, "Unknown flag: -%c\n", argv[i][j]);
                        print_help();
                        return 1;
                }
            }
        } else {
            // Not a flag - must be pattern or directory
            if (pattern == NULL) {
                pattern = argv[i];
            } else {
                directory = argv[i];
            }
        }
    }

    if (pattern == NULL) {
        fprintf(stderr, "Error: No search pattern specified.\n");
        print_help();
        return 1;
    }

    // Get input from stdin if available (for pipe functionality)
    char buffer[4096];
    if (!isatty(STDIN_FILENO)) {
        // Input is coming from a pipe
        FILE *temp = tmpfile();
        if (!temp) {
            perror("Failed to create temporary file");
            return 1;
        }

        // Read input from stdin and store in temporary file
        while (fgets(buffer, sizeof(buffer), stdin)) {
            fputs(buffer, temp);
        }
        
        rewind(temp);
        
        // Search in the temporary file
        fseek(temp, 0, SEEK_SET);
        int line_number = 1;
        int match_count = 0;
        
        while (fgets(buffer, sizeof(buffer), temp)) {
            if (line_contains_pattern(buffer, pattern, case_insensitive)) {
                match_count++;
                if (!count_only) {
                    if (show_line_numbers) {
                        printf("%d: ", line_number);
                    }
                    printf("%s", buffer);
                    if (buffer[strlen(buffer) - 1] != '\n') {
                        printf("\n");
                    }
                }
            }
            line_number++;
        }
        
        if (count_only) {
            printf("Matches found: %d\n", match_count);
        }
        
        fclose(temp);
    } else {
        // Input is from command line arguments
        if (is_regular_file(directory)) {
            // If directory is actually a file, search in that file
            search_in_file(directory, pattern, case_insensitive, show_line_numbers, count_only);
        } else {
            // Search in the directory
            search_in_directory(directory, pattern, recursive, case_insensitive, show_line_numbers, count_only);
        }
    }

    return 0;
}

// Check if a line contains the pattern (case insensitive if specified)
int line_contains_pattern(const char *line, const char *pattern, int case_insensitive) {
    if (case_insensitive) {
        char *line_lower = strdup(line);
        char *pattern_lower = strdup(pattern);
        if (!line_lower || !pattern_lower) {
            perror("Memory allocation failed");
            exit(1);
        }
        
        // Convert to lowercase
        for (size_t i = 0; i < strlen(line_lower); i++) {
            line_lower[i] = tolower(line_lower[i]);
        }
        for (size_t i = 0; i < strlen(pattern_lower); i++) {
            pattern_lower[i] = tolower(pattern_lower[i]);
        }
        
        int result = strstr(line_lower, pattern_lower) != NULL;
        free(line_lower);
        free(pattern_lower);
        return result;
    } else {
        return strstr(line, pattern) != NULL;
    }
}

// Check if a path is a regular file
int is_regular_file(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return 0;
    }
    return S_ISREG(path_stat.st_mode);
}

// Search for pattern in a specific file
void search_in_file(const char *file_path, const char *pattern, int case_insensitive, int show_line_numbers, int count_only) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "Cannot open file '%s': %s\n", file_path, strerror(errno));
        return;
    }

    char line[4096];
    int line_number = 1;
    int match_count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line_contains_pattern(line, pattern, case_insensitive)) {
            match_count++;
            if (!count_only) {
                printf("%s:", file_path);
                if (show_line_numbers) {
                    printf("%d:", line_number);
                }
                printf(" %s", line);
                if (line[strlen(line) - 1] != '\n') {
                    printf("\n");
                }
            }
        }
        line_number++;
    }

    if (count_only) {
        printf("%s: %d matches\n", file_path, match_count);
    }

    fclose(file);
}

// Search for pattern in a directory
void search_in_directory(const char *dir_path, const char *pattern, int recursive, int case_insensitive, int show_line_numbers, int count_only) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "Cannot open directory '%s': %s\n", dir_path, strerror(errno));
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/%s", dir_path, entry->d_name);

        if (is_regular_file(path)) {
            search_in_file(path, pattern, case_insensitive, show_line_numbers, count_only);
        } else if (recursive) {
            // Check if it's a directory first
            struct stat statbuf;
            if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
                search_in_directory(path, pattern, recursive, case_insensitive, show_line_numbers, count_only);
            }
        }
    }

    closedir(dir);
}

// Print help message
void print_help() {
    printf("\nCARIDONG - File content search command\n");
    printf("Usage: caridong {pattern} [directory]\n\n");
    printf("Description: Searches for files containing a specific pattern\n\n");
    printf("Flags:\n");
    printf("  -i : Case insensitive search\n");
    printf("  -r : Recursive search in subdirectories\n");
    printf("  -n : Show line numbers with matches\n");
    printf("  -c : Count matches only\n\n");
    printf("Examples:\n");
    printf("  caridong \"hello\" : Search for 'hello' in current directory\n");
    printf("  caridong -i \"Hello\" ./src : Case insensitive search for 'Hello' in ./src\n");
    printf("  caridong -rn \"import\" ./project : Recursive search with line numbers\n");
    printf("  caridong -c \"error\" ./logs : Count occurrences only\n");
    printf("  somecommand | caridong \"pattern\" : Search in piped input\n\n");
    printf("Flags can be combined: caridong -ir \"pattern\"\n");
}