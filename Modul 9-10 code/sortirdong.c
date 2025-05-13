#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1024

typedef struct {
    char *line;
    int numeric_value;  // Used for numeric sorting
} SortLine;

void print_help() {
    printf("\n===== SORTIRDONG HELP =====\n");
    printf("Usage: sortirdong {filename} [column]\n");
    printf("Description: Sort file contents\n\n");
    printf("Flags:\n");
    printf("  -n: Sort numerically\n");
    printf("  -r: Sort in reverse order\n");
    printf("  -u: Remove duplicates\n");
    printf("  -f: Case insensitive sorting\n");
    printf("  -h, --help: Show this help message\n\n");
    printf("Examples:\n");
    printf("  sortirdong data.txt: Sort lines in data.txt alphabetically\n");
    printf("  sortirdong -nr data.csv 2: Sort data.csv numerically by column 2 in reverse order\n");
    printf("  sortirdong -uf names.txt: Sort names.txt case insensitively and remove duplicates\n");
}

// Compare function for alphabetical sorting
int compare_alphabetical(const void *a, const void *b) {
    SortLine *line_a = (SortLine *)a;
    SortLine *line_b = (SortLine *)b;
    return strcmp(line_a->line, line_b->line);
}

// Compare function for numeric sorting
int compare_numeric(const void *a, const void *b) {
    SortLine *line_a = (SortLine *)a;
    SortLine *line_b = (SortLine *)b;
    return line_a->numeric_value - line_b->numeric_value;
}

// Case insensitive compare function
int compare_case_insensitive(const void *a, const void *b) {
    SortLine *line_a = (SortLine *)a;
    SortLine *line_b = (SortLine *)b;
    
    char *str_a = strdup(line_a->line);
    char *str_b = strdup(line_b->line);
    
    for (int i = 0; str_a[i]; i++) {
        str_a[i] = tolower(str_a[i]);
    }
    
    for (int i = 0; str_b[i]; i++) {
        str_b[i] = tolower(str_b[i]);
    }
    
    int result = strcmp(str_a, str_b);
    
    free(str_a);
    free(str_b);
    
    return result;
}

// Extract column from a line
char *extract_column(char *line, int column) {
    char *token;
    char *line_copy = strdup(line);
    int curr_col = 1;
    
    token = strtok(line_copy, " \t,");
    while (token != NULL && curr_col < column) {
        token = strtok(NULL, " \t,");
        curr_col++;
    }
    
    char *result = token ? strdup(token) : strdup("");
    free(line_copy);
    return result;
}

// Extract numeric value from a column
int extract_numeric_value(char *line, int column) {
    char *col_text = extract_column(line, column);
    int value = atoi(col_text);
    free(col_text);
    return value;
}

int main(int argc, char *argv[]) {
    int numeric_sort = 0;
    int reverse_order = 0;
    int remove_duplicates = 0;
    int case_insensitive = 0;
    char *filename = NULL;
    int column = 1;  // Default to first column
    
    // Check for help flag
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
    }
    
    // Parse flags and arguments
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] != '-') {
            // It's a flag
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'n': numeric_sort = 1; break;
                    case 'r': reverse_order = 1; break;
                    case 'u': remove_duplicates = 1; break;
                    case 'f': case_insensitive = 1; break;
                    case 'h': print_help(); return 0;
                    default:
                        fprintf(stderr, "Unknown flag: %c\n", argv[i][j]);
                        print_help();
                        return 1;
                }
            }
        } else if (strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        } else if (filename == NULL) {
            filename = argv[i];
        } else {
            column = atoi(argv[i]);
            if (column <= 0) {
                fprintf(stderr, "Invalid column number: %s\n", argv[i]);
                return 1;
            }
        }
    }
    
    if (filename == NULL) {
        fprintf(stderr, "No input file provided\n");
        print_help();
        return 1;
    }
    
    // Open the file
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Cannot open file");
        return 1;
    }
    
    // Read lines from file
    SortLine lines[MAX_LINES];
    char buffer[MAX_LINE_LENGTH];
    int line_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && line_count < MAX_LINES) {
        // Remove trailing newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        
        lines[line_count].line = strdup(buffer);
        if (numeric_sort) {
            lines[line_count].numeric_value = extract_numeric_value(buffer, column);
        }
        line_count++;
    }
    
    fclose(file);
    
    // Sort the lines
    if (numeric_sort) {
        qsort(lines, line_count, sizeof(SortLine), compare_numeric);
    } else if (case_insensitive) {
        qsort(lines, line_count, sizeof(SortLine), compare_case_insensitive);
    } else {
        qsort(lines, line_count, sizeof(SortLine), compare_alphabetical);
    }
    
    // Print sorted lines
    if (remove_duplicates) {
        printf("%s\n", lines[0].line);
        for (int i = 1; i < line_count; i++) {
            int is_duplicate = 0;
            
            if (case_insensitive) {
                char *current = strdup(lines[i].line);
                char *prev = strdup(lines[i-1].line);
                
                for (int j = 0; current[j]; j++) current[j] = tolower(current[j]);
                for (int j = 0; prev[j]; j++) prev[j] = tolower(prev[j]);
                
                is_duplicate = (strcmp(current, prev) == 0);
                
                free(current);
                free(prev);
            } else {
                is_duplicate = (strcmp(lines[i].line, lines[i-1].line) == 0);
            }
            
            if (!is_duplicate) {
                if (reverse_order) {
                    printf("%s\n", lines[line_count - 1 - i].line);
                } else {
                    printf("%s\n", lines[i].line);
                }
            }
        }
    } else {
        if (reverse_order) {
            for (int i = line_count - 1; i >= 0; i--) {
                printf("%s\n", lines[i].line);
            }
        } else {
            for (int i = 0; i < line_count; i++) {
                printf("%s\n", lines[i].line);
            }
        }
    }
    
    // Free allocated memory
    for (int i = 0; i < line_count; i++) {
        free(lines[i].line);
    }
    
    return 0;
}