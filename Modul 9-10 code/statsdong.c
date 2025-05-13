#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1024
#define MAX_COLUMNS 50

typedef struct {
    double min;
    double max;
    double sum;
    double mean;
    double median;
    double mode;
    double mode_count;
    double std_dev;
    int count;
    double values[MAX_LINES];
} Stats;

void print_help() {
    printf("\n===== STATSDONG HELP =====\n");
    printf("Usage: statsdong {filename}\n");
    printf("Description: Perform statistical analysis on data file\n\n");
    printf("Flags:\n");
    printf("  -a: Calculate mean, median, mode\n");
    printf("  -g: Generate graph of data distribution\n");
    printf("  -c {column}: Select column for analysis\n");
    printf("  -n: Normalize data (0-1 range)\n");
    printf("  -h, --help: Show this help message\n\n");
    printf("Examples:\n");
    printf("  statsdong data.txt: Basic stats for data.txt\n");
    printf("  statsdong -a data.csv: Calculate all statistics\n");
    printf("  statsdong -c 2 grades.csv: Analyze column 2 of grades.csv\n");
    printf("  statsdong -agn temperature.txt: Full analysis with graph and normalization\n");
}

// Parse a line and extract value from specified column
double parse_line(char *line, int column) {
    char *token;
    int curr_col = 1;
    double value = 0.0;
    char *line_copy = strdup(line);
    
    token = strtok(line_copy, " \t,");
    while (token != NULL && curr_col <= column) {
        if (curr_col == column) {
            value = atof(token);
            break;
        }
        token = strtok(NULL, " \t,");
        curr_col++;
    }
    
    free(line_copy);
    return value;
}

// Calculate statistics for an array of values
void calculate_stats(Stats *stats) {
    if (stats->count == 0) {
        return;
    }
    
    // Calculate mean
    stats->mean = stats->sum / stats->count;
    
    // Calculate median (sort values first)
    for (int i = 0; i < stats->count - 1; i++) {
        for (int j = 0; j < stats->count - i - 1; j++) {
            if (stats->values[j] > stats->values[j + 1]) {
                double temp = stats->values[j];
                stats->values[j] = stats->values[j + 1];
                stats->values[j + 1] = temp;
            }
        }
    }
    
    if (stats->count % 2 == 0) {
        stats->median = (stats->values[stats->count / 2 - 1] + stats->values[stats->count / 2]) / 2;
    } else {
        stats->median = stats->values[stats->count / 2];
    }
    
    // Calculate mode
    double current_value = stats->values[0];
    int current_count = 1;
    stats->mode = current_value;
    stats->mode_count = current_count;
    
    for (int i = 1; i < stats->count; i++) {
        if (stats->values[i] == current_value) {
            current_count++;
        } else {
            if (current_count > stats->mode_count) {
                stats->mode = current_value;
                stats->mode_count = current_count;
            }
            current_value = stats->values[i];
            current_count = 1;
        }
    }
    
    // Check last set of values
    if (current_count > stats->mode_count) {
        stats->mode = current_value;
        stats->mode_count = current_count;
    }
    
    // Calculate standard deviation
    double sum_squared_diff = 0;
    for (int i = 0; i < stats->count; i++) {
        sum_squared_diff += pow(stats->values[i] - stats->mean, 2);
    }
    stats->std_dev = sqrt(sum_squared_diff / stats->count);
}

// Normalize values to 0-1 range
void normalize_values(Stats *stats) {
    if (stats->count == 0 || stats->max == stats->min) {
        return;
    }
    
    double range = stats->max - stats->min;
    for (int i = 0; i < stats->count; i++) {
        stats->values[i] = (stats->values[i] - stats->min) / range;
    }
    
    // Update stats
    stats->min = 0;
    stats->max = 1;
    stats->sum = 0;
    for (int i = 0; i < stats->count; i++) {
        stats->sum += stats->values[i];
    }
    
    calculate_stats(stats);
}

// Generate ASCII histogram
void generate_histogram(Stats *stats, int width, int height) {
    if (stats->count < 2) {
        printf("Not enough data for histogram\n");
        return;
    }
    
    int bins[50] = {0};
    int max_bin_count = 0;
    int num_bins = width < 50 ? width : 50;
    
    // Determine bin size and fill bins
    double bin_size = (stats->max - stats->min) / num_bins;
    
    for (int i = 0; i < stats->count; i++) {
        int bin = (int)((stats->values[i] - stats->min) / bin_size);
        if (bin >= num_bins) bin = num_bins - 1;
        bins[bin]++;
        if (bins[bin] > max_bin_count) {
            max_bin_count = bins[bin];
        }
    }
    
    // Draw histogram
    printf("\nData Distribution:\n");
    
    // Scale to fit height
    double scale_factor = (max_bin_count > height) ? (double)height / max_bin_count : 1.0;
    
    // Draw y-axis labels and bars
    for (int h = height; h > 0; h--) {
        if (h == height) {
            printf("%3d |", max_bin_count);
        } else if (h == height / 2) {
            printf("%3d |", max_bin_count / 2);
        } else if (h == 1) {
            printf("%3d |", 0);
        } else {
            printf("    |");
        }
        
        for (int b = 0; b < num_bins; b++) {
            int scaled_height = (int)(bins[b] * scale_factor);
            if (scaled_height >= h) {
                printf("#");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    
    // Draw x-axis
    printf("    +");
    for (int b = 0; b < num_bins; b++) {
        printf("-");
    }
    printf("\n");
    
    // Draw x-axis labels
    printf("    ");
    printf("%.1f", stats->min);
    int label_space = (num_bins - 10) / 2;
    for (int i = 0; i < label_space; i++) {
        printf(" ");
    }
    printf("%.1f", (stats->min + stats->max) / 2);
    for (int i = 0; i < label_space; i++) {
        printf(" ");
    }
    printf("%.1f\n", stats->max);
}

int main(int argc, char *argv[]) {
    char *filename = NULL;
    int column = 1;
    int calculate_all = 0;
    int generate_graph = 0;
    int normalize = 0;
    
    // Check for help flag
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
    }
    
    // Parse flags and arguments
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] != '\0') {
            // It's a flag
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'a': calculate_all = 1; break;
                    case 'g': generate_graph = 1; break;
                    case 'n': normalize = 1; break;
                    case 'c': 
                        if (i + 1 < argc) {
                            column = atoi(argv[i + 1]);
                            if (column <= 0) {
                                fprintf(stderr, "Invalid column: %s\n", argv[i + 1]);
                                return 1;
                            }
                            i++;  // Skip the next argument as it's the column number
                        } else {
                            fprintf(stderr, "No column number provided\n");
                            return 1;
                        }
                        break;
                    case 'h': print_help(); return 0;
                    default:
                        fprintf(stderr, "Unknown flag: %c\n", argv[i][j]);
                        print_help();
                        return 1;
                }
            }
        } else if (filename == NULL) {
            filename = argv[i];
        }
    }
    
    if (filename == NULL) {
        fprintf(stderr, "No input file provided\n");
        print_help();
        return 1;
    }
    
    // Open file
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Cannot open file");
        return 1;
    }
    
    // Initialize stats
    Stats stats;
    stats.min = INFINITY;
    stats.max = -INFINITY;
    stats.sum = 0;
    stats.count = 0;
    
    // Read and process data
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) && stats.count < MAX_LINES) {
        // Skip empty lines or comments
        if (line[0] == '\n' || line[0] == '#') {
            continue;
        }
        
        // Extract data from the column
        double value = parse_line(line, column);
        
        // Add to stats
        stats.values[stats.count] = value;
        stats.sum += value;
        if (value < stats.min) stats.min = value;
        if (value > stats.max) stats.max = value;
        stats.count++;
    }
    
    fclose(file);
    
    if (stats.count == 0) {
        printf("No data found in the file or column\n");
        return 1;
    }
    
    // Normalize if requested
    if (normalize) {
        normalize_values(&stats);
    }
    
    // Calculate statistics
    calculate_stats(&stats);
    
    // Print results
    printf("\n===== Statistics for %s", filename);
    if (column > 1) {
        printf(" (Column %d)", column);
    }
    printf(" =====\n");
    
    printf("Count: %d\n", stats.count);
    printf("Min: %.4f\n", stats.min);
    printf("Max: %.4f\n", stats.max);
    printf("Range: %.4f\n", stats.max - stats.min);
    printf("Sum: %.4f\n", stats.sum);
    printf("Mean: %.4f\n", stats.mean);
    
    if (calculate_all) {
        printf("Median: %.4f\n", stats.median);
        printf("Mode: %.4f (occurs %.0f times)\n", stats.mode, stats.mode_count);
        printf("Standard Deviation: %.4f\n", stats.std_dev);
        printf("Variance: %.4f\n", stats.std_dev * stats.std_dev);
    }
    
    // Generate histogram if requested
    if (generate_graph) {
        generate_histogram(&stats, 50, 15);
    }
    
    return 0;
}